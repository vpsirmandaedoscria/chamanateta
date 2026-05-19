"""
Gerenciador Fazcode - Process Manager & Restart on Crash
Dark/Pink Neon themed process monitoring and auto-restart tool.
"""
import json
import os
import signal
import subprocess
import sys
import threading
import time
import webbrowser
import xml.etree.ElementTree as ET
from datetime import datetime
from io import BytesIO
from pathlib import Path
from xml.dom import minidom

import psutil
from flask import Flask, jsonify, render_template, request, send_file


def resource_path(relative_path):
    """Get absolute path to resource, works for dev and PyInstaller."""
    if getattr(sys, "frozen", False):
        base_path = Path(sys._MEIPASS)
    else:
        base_path = Path(__file__).parent
    return str(base_path / relative_path)


if getattr(sys, "frozen", False):
    template_folder = resource_path("templates")
    static_folder = resource_path("static")
    app = Flask(__name__, template_folder=template_folder, static_folder=static_folder)
else:
    app = Flask(__name__)

DATA_DIR = Path(os.path.expanduser("~")) / "GerenciadorFazcode" if getattr(sys, "frozen", False) else Path(__file__).parent / "data"
DATA_DIR.mkdir(parents=True, exist_ok=True)
SETTINGS_FILE = DATA_DIR / "settings.json"
PROCESSES_FILE = DATA_DIR / "processes.json"
BEC_FILE = DATA_DIR / "bec_scheduler.json"
LOG_FILE = DATA_DIR / "activity.log"

DEFAULT_SETTINGS = {
    "check_interval": 5,
    "max_log_lines": 500,
    "auto_start_monitoring": False,
    "minimize_to_tray": True,
    "sound_on_restart": True,
    "max_restart_attempts": 10,
    "restart_cooldown": 3,
}

monitor_thread = None
monitoring_active = False
lock = threading.Lock()


def load_json(filepath, default):
    try:
        if filepath.exists():
            with open(filepath, "r", encoding="utf-8") as f:
                return json.load(f)
    except (json.JSONDecodeError, IOError):
        pass
    return default


def save_json(filepath, data):
    with open(filepath, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)


def add_log(message, level="INFO"):
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    entry = f"[{timestamp}] [{level}] {message}\n"
    try:
        with open(LOG_FILE, "a", encoding="utf-8") as f:
            f.write(entry)
    except IOError:
        pass


def get_logs(limit=100):
    try:
        if LOG_FILE.exists():
            with open(LOG_FILE, "r", encoding="utf-8") as f:
                lines = f.readlines()
            return lines[-limit:]
    except IOError:
        pass
    return []


def clear_logs():
    try:
        with open(LOG_FILE, "w", encoding="utf-8") as f:
            f.write("")
    except IOError:
        pass


def is_process_running(name=None, pid=None):
    """Check if a process is running by name or PID."""
    if pid:
        try:
            proc = psutil.Process(pid)
            return proc.is_running() and proc.status() != psutil.STATUS_ZOMBIE
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            return False

    if name:
        name_lower = name.lower()
        for proc in psutil.process_iter(["name", "exe"]):
            try:
                proc_name = proc.info.get("name", "")
                proc_exe = proc.info.get("exe", "") or ""
                if proc_name and proc_name.lower() == name_lower:
                    return True
                if proc_exe and name_lower in proc_exe.lower():
                    return True
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
    return False


def find_process_pid(name):
    """Find PID of a running process by name."""
    name_lower = name.lower()
    for proc in psutil.process_iter(["name", "exe", "pid"]):
        try:
            proc_name = proc.info.get("name", "")
            proc_exe = proc.info.get("exe", "") or ""
            if proc_name and proc_name.lower() == name_lower:
                return proc.info["pid"]
            if proc_exe and name_lower in proc_exe.lower():
                return proc.info["pid"]
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue
    return None


def restart_process(proc_config):
    """Restart a monitored process."""
    command = proc_config.get("command", "")
    working_dir = proc_config.get("working_dir", "")
    name = proc_config.get("name", "Unknown")

    if not command:
        add_log(f"No command configured for '{name}'", "ERROR")
        return False

    try:
        kwargs = {
            "shell": True,
            "start_new_session": True,
        }
        if working_dir and os.path.isdir(working_dir):
            kwargs["cwd"] = working_dir

        subprocess.Popen(command, **kwargs)
        add_log(f"Process '{name}' restarted successfully", "SUCCESS")
        proc_config["restart_count"] = proc_config.get("restart_count", 0) + 1
        proc_config["last_restart"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        return True
    except Exception as e:
        add_log(f"Failed to restart '{name}': {str(e)}", "ERROR")
        return False


def monitor_loop():
    """Main monitoring loop that checks processes and restarts if needed."""
    global monitoring_active
    add_log("Monitoring started", "INFO")

    while monitoring_active:
        settings = load_json(SETTINGS_FILE, DEFAULT_SETTINGS)
        processes = load_json(PROCESSES_FILE, [])
        changed = False

        for proc in processes:
            if not proc.get("enabled", True):
                continue

            name = proc.get("name", "")
            process_name = proc.get("process_name", "")
            lookup = process_name or name

            if not lookup:
                continue

            running = is_process_running(name=lookup, pid=proc.get("tracked_pid"))
            prev_status = proc.get("status", "unknown")

            if running:
                proc["status"] = "running"
                pid = find_process_pid(lookup)
                if pid:
                    proc["tracked_pid"] = pid
                proc["consecutive_failures"] = 0
            else:
                proc["status"] = "stopped"
                proc["tracked_pid"] = None

                if proc.get("auto_restart", True) and prev_status == "running":
                    max_attempts = settings.get("max_restart_attempts", 10)
                    failures = proc.get("consecutive_failures", 0)

                    if failures < max_attempts:
                        add_log(
                            f"Process '{name}' crashed! Attempting restart "
                            f"({failures + 1}/{max_attempts})...",
                            "WARNING",
                        )
                        cooldown = settings.get("restart_cooldown", 3)
                        time.sleep(cooldown)

                        if restart_process(proc):
                            proc["consecutive_failures"] = failures + 1
                            time.sleep(2)
                            if is_process_running(name=lookup):
                                proc["status"] = "running"
                                pid = find_process_pid(lookup)
                                if pid:
                                    proc["tracked_pid"] = pid
                        else:
                            proc["consecutive_failures"] = failures + 1
                    else:
                        add_log(
                            f"Process '{name}' exceeded max restart attempts "
                            f"({max_attempts}). Disabling auto-restart.",
                            "ERROR",
                        )
                        proc["auto_restart"] = False

            if proc.get("status") != prev_status:
                changed = True

        if changed:
            save_json(PROCESSES_FILE, processes)

        interval = settings.get("check_interval", 5)
        for _ in range(int(interval * 10)):
            if not monitoring_active:
                break
            time.sleep(0.1)

    add_log("Monitoring stopped", "INFO")


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/processes", methods=["GET"])
def get_processes():
    processes = load_json(PROCESSES_FILE, [])
    for proc in processes:
        lookup = proc.get("process_name") or proc.get("name", "")
        if lookup:
            proc["status"] = (
                "running"
                if is_process_running(name=lookup, pid=proc.get("tracked_pid"))
                else "stopped"
            )
    return jsonify(processes)


@app.route("/api/processes", methods=["POST"])
def add_process():
    data = request.json
    if not data or not data.get("name"):
        return jsonify({"error": "Name is required"}), 400

    processes = load_json(PROCESSES_FILE, [])
    new_proc = {
        "id": int(time.time() * 1000),
        "name": data["name"],
        "process_name": data.get("process_name", ""),
        "command": data.get("command", ""),
        "working_dir": data.get("working_dir", ""),
        "arguments": data.get("arguments", ""),
        "enabled": data.get("enabled", True),
        "auto_restart": data.get("auto_restart", True),
        "status": "unknown",
        "tracked_pid": None,
        "restart_count": 0,
        "consecutive_failures": 0,
        "last_restart": None,
        "added_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
    }
    processes.append(new_proc)
    save_json(PROCESSES_FILE, processes)
    add_log(f"Process '{data['name']}' added to monitor list", "INFO")
    return jsonify(new_proc), 201


@app.route("/api/processes/<int:proc_id>", methods=["PUT"])
def update_process(proc_id):
    data = request.json
    processes = load_json(PROCESSES_FILE, [])

    for proc in processes:
        if proc["id"] == proc_id:
            for key in [
                "name",
                "process_name",
                "command",
                "working_dir",
                "arguments",
                "enabled",
                "auto_restart",
            ]:
                if key in data:
                    proc[key] = data[key]
            save_json(PROCESSES_FILE, processes)
            add_log(f"Process '{proc['name']}' updated", "INFO")
            return jsonify(proc)

    return jsonify({"error": "Process not found"}), 404


@app.route("/api/processes/<int:proc_id>", methods=["DELETE"])
def delete_process(proc_id):
    processes = load_json(PROCESSES_FILE, [])
    original_len = len(processes)
    processes = [p for p in processes if p["id"] != proc_id]

    if len(processes) < original_len:
        save_json(PROCESSES_FILE, processes)
        add_log(f"Process removed from monitor list (ID: {proc_id})", "INFO")
        return jsonify({"success": True})

    return jsonify({"error": "Process not found"}), 404


@app.route("/api/processes/<int:proc_id>/restart", methods=["POST"])
def manual_restart(proc_id):
    processes = load_json(PROCESSES_FILE, [])

    for proc in processes:
        if proc["id"] == proc_id:
            add_log(f"Manual restart requested for '{proc['name']}'", "INFO")
            success = restart_process(proc)
            save_json(PROCESSES_FILE, processes)
            return jsonify({"success": success})

    return jsonify({"error": "Process not found"}), 404


@app.route("/api/processes/<int:proc_id>/kill", methods=["POST"])
def kill_process(proc_id):
    processes = load_json(PROCESSES_FILE, [])

    for proc in processes:
        if proc["id"] == proc_id:
            lookup = proc.get("process_name") or proc.get("name", "")
            pid = proc.get("tracked_pid") or find_process_pid(lookup)
            if pid:
                try:
                    os.kill(pid, signal.SIGTERM)
                    add_log(f"Process '{proc['name']}' (PID: {pid}) terminated", "WARNING")
                    proc["status"] = "stopped"
                    proc["tracked_pid"] = None
                    save_json(PROCESSES_FILE, processes)
                    return jsonify({"success": True})
                except (ProcessLookupError, PermissionError) as e:
                    add_log(f"Failed to kill '{proc['name']}': {str(e)}", "ERROR")
                    return jsonify({"error": str(e)}), 500
            return jsonify({"error": "Process not running"}), 404

    return jsonify({"error": "Process not found"}), 404


@app.route("/api/monitoring/start", methods=["POST"])
def start_monitoring():
    global monitor_thread, monitoring_active

    with lock:
        if monitoring_active:
            return jsonify({"status": "already_running"})

        monitoring_active = True
        monitor_thread = threading.Thread(target=monitor_loop, daemon=True)
        monitor_thread.start()

    return jsonify({"status": "started"})


@app.route("/api/monitoring/stop", methods=["POST"])
def stop_monitoring():
    global monitoring_active

    with lock:
        monitoring_active = False

    return jsonify({"status": "stopped"})


@app.route("/api/monitoring/status", methods=["GET"])
def monitoring_status():
    return jsonify({"active": monitoring_active})


@app.route("/api/settings", methods=["GET"])
def get_settings():
    settings = load_json(SETTINGS_FILE, DEFAULT_SETTINGS)
    merged = {**DEFAULT_SETTINGS, **settings}
    return jsonify(merged)


@app.route("/api/settings", methods=["PUT"])
def update_settings():
    data = request.json
    settings = load_json(SETTINGS_FILE, DEFAULT_SETTINGS)
    settings.update(data)
    save_json(SETTINGS_FILE, settings)
    add_log("Settings updated", "INFO")
    return jsonify(settings)


@app.route("/api/logs", methods=["GET"])
def get_activity_logs():
    limit = request.args.get("limit", 100, type=int)
    logs = get_logs(limit)
    return jsonify({"logs": logs})


@app.route("/api/logs", methods=["DELETE"])
def delete_logs():
    clear_logs()
    return jsonify({"success": True})


@app.route("/api/system/processes", methods=["GET"])
def list_system_processes():
    """List running system processes for easy selection."""
    search = request.args.get("search", "").lower()
    procs = []
    seen = set()

    for proc in psutil.process_iter(["pid", "name", "exe", "status", "cpu_percent", "memory_info"]):
        try:
            info = proc.info
            name = info.get("name", "")
            if not name or name in seen:
                continue
            if search and search not in name.lower():
                continue
            seen.add(name)
            mem = info.get("memory_info")
            procs.append(
                {
                    "pid": info["pid"],
                    "name": name,
                    "exe": info.get("exe", "") or "",
                    "status": info.get("status", ""),
                    "memory_mb": round(mem.rss / 1024 / 1024, 1) if mem else 0,
                }
            )
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue

    procs.sort(key=lambda x: x["name"].lower())
    return jsonify(procs[:100])


# ===== BEC Scheduler =====

DEFAULT_BEC = {
    "restart_times": ["06:00", "12:00", "18:00", "00:00"],
    "days": [1, 2, 3, 4, 5, 6, 7],
    "warnings": [
        {"minutes_before": 30, "message": "SERVIDOR REINICIA EM 30 MINUTOS"},
        {"minutes_before": 15, "message": "SERVIDOR REINICIA EM 15 MINUTOS"},
        {"minutes_before": 5, "message": "SERVIDOR REINICIA EM 5 MINUTOS"},
        {"minutes_before": 3, "message": "SERVIDOR REINICIA EM 3 MINUTOS"},
        {"minutes_before": 1, "message": "SERVIDOR REINICIA EM 1 MINUTO - SAIA AGORA PARA NAO PERDER SEU GEAR"},
    ],
    "kick_before_restart": True,
    "kick_minutes_before": 1,
    "kick_message": "Servidor reiniciando! Voce sera reconectado em breve.",
    "shutdown_command": "#shutdown",
    "lock_before_restart": True,
    "lock_minutes_before": 2,
    "custom_jobs": [],
}

DAY_NAMES = {1: "Segunda", 2: "Terca", 3: "Quarta", 4: "Quinta", 5: "Sexta", 6: "Sabado", 7: "Domingo"}


def subtract_minutes(time_str, minutes):
    """Subtract minutes from HH:MM time, returns HH:MM:SS."""
    h, m = map(int, time_str.split(":"))
    total = h * 60 + m - minutes
    if total < 0:
        total += 1440
    new_h = (total // 60) % 24
    new_m = total % 60
    return f"{new_h:02d}:{new_m:02d}:00"


def generate_bec_xml(bec_data):
    """Generate BEC scheduler.xml from config."""
    root = ET.Element("Scheduler")
    job_id = 0
    days_str = ",".join(str(d) for d in sorted(bec_data.get("days", [1, 2, 3, 4, 5, 6, 7])))

    for restart_time in bec_data.get("restart_times", []):
        warnings = sorted(bec_data.get("warnings", []), key=lambda w: w["minutes_before"], reverse=True)

        # Warning messages
        for warning in warnings:
            job = ET.SubElement(root, "job", id=str(job_id))
            ET.SubElement(job, "time").text = subtract_minutes(restart_time, warning["minutes_before"])
            ET.SubElement(job, "delay").text = "000000"
            ET.SubElement(job, "day").text = days_str
            ET.SubElement(job, "loop").text = "0"
            ET.SubElement(job, "cmd").text = f"say -1 {warning['message']}"
            ET.SubElement(job, "cmdtype").text = "0"
            job_id += 1

        # Lock server before restart
        if bec_data.get("lock_before_restart", False):
            lock_mins = bec_data.get("lock_minutes_before", 2)
            job = ET.SubElement(root, "job", id=str(job_id))
            ET.SubElement(job, "time").text = subtract_minutes(restart_time, lock_mins)
            ET.SubElement(job, "delay").text = "000000"
            ET.SubElement(job, "day").text = days_str
            ET.SubElement(job, "loop").text = "0"
            ET.SubElement(job, "cmd").text = "#lock"
            ET.SubElement(job, "cmdtype").text = "0"
            job_id += 1

        # Kick all players before restart
        if bec_data.get("kick_before_restart", False):
            kick_mins = bec_data.get("kick_minutes_before", 1)
            kick_msg = bec_data.get("kick_message", "Servidor reiniciando!")
            job = ET.SubElement(root, "job", id=str(job_id))
            ET.SubElement(job, "time").text = subtract_minutes(restart_time, kick_mins)
            ET.SubElement(job, "delay").text = "000000"
            ET.SubElement(job, "day").text = days_str
            ET.SubElement(job, "loop").text = "0"
            ET.SubElement(job, "cmd").text = f"kick -1 {kick_msg}"
            ET.SubElement(job, "cmdtype").text = "0"
            job_id += 1

        # Shutdown/Restart command
        shutdown_cmd = bec_data.get("shutdown_command", "#shutdown")
        job = ET.SubElement(root, "job", id=str(job_id))
        ET.SubElement(job, "time").text = f"{restart_time}:00"
        ET.SubElement(job, "delay").text = "000000"
        ET.SubElement(job, "day").text = days_str
        ET.SubElement(job, "loop").text = "0"
        ET.SubElement(job, "cmd").text = shutdown_cmd
        ET.SubElement(job, "cmdtype").text = "0"
        job_id += 1

        # Unlock after restart (if lock was used)
        if bec_data.get("lock_before_restart", False):
            # Unlock 1 minute after restart time
            h, m = map(int, restart_time.split(":"))
            unlock_m = m + 1
            unlock_h = h
            if unlock_m >= 60:
                unlock_m -= 60
                unlock_h = (unlock_h + 1) % 24
            job = ET.SubElement(root, "job", id=str(job_id))
            ET.SubElement(job, "time").text = f"{unlock_h:02d}:{unlock_m:02d}:00"
            ET.SubElement(job, "delay").text = "000000"
            ET.SubElement(job, "day").text = days_str
            ET.SubElement(job, "loop").text = "0"
            ET.SubElement(job, "cmd").text = "#unlock"
            ET.SubElement(job, "cmdtype").text = "0"
            job_id += 1

    # Custom jobs
    for custom in bec_data.get("custom_jobs", []):
        job = ET.SubElement(root, "job", id=str(job_id))
        ET.SubElement(job, "time").text = custom.get("time", "00:00:00")
        ET.SubElement(job, "delay").text = custom.get("delay", "000000")
        ET.SubElement(job, "day").text = custom.get("day", days_str)
        ET.SubElement(job, "loop").text = str(custom.get("loop", 0))
        ET.SubElement(job, "cmd").text = custom.get("cmd", "")
        ET.SubElement(job, "cmdtype").text = str(custom.get("cmdtype", 0))
        job_id += 1

    rough_string = ET.tostring(root, encoding="unicode")
    parsed = minidom.parseString(rough_string)
    return parsed.toprettyxml(indent="    ", encoding=None)


@app.route("/api/bec", methods=["GET"])
def get_bec():
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    merged = {**DEFAULT_BEC, **bec}
    return jsonify(merged)


@app.route("/api/bec", methods=["PUT"])
def update_bec():
    data = request.json
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    bec.update(data)
    save_json(BEC_FILE, bec)
    add_log("BEC Scheduler atualizado", "INFO")
    return jsonify(bec)


@app.route("/api/bec/restart-times", methods=["POST"])
def add_restart_time():
    data = request.json
    time_val = data.get("time", "")
    if not time_val:
        return jsonify({"error": "Time is required"}), 400

    bec = load_json(BEC_FILE, DEFAULT_BEC)
    times = bec.get("restart_times", [])
    if time_val not in times:
        times.append(time_val)
        times.sort()
        bec["restart_times"] = times
        save_json(BEC_FILE, bec)
        add_log(f"Restart time {time_val} adicionado", "INFO")
    return jsonify(bec)


@app.route("/api/bec/restart-times", methods=["DELETE"])
def remove_restart_time():
    data = request.json
    time_val = data.get("time", "")
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    times = bec.get("restart_times", [])
    if time_val in times:
        times.remove(time_val)
        bec["restart_times"] = times
        save_json(BEC_FILE, bec)
        add_log(f"Restart time {time_val} removido", "INFO")
    return jsonify(bec)


@app.route("/api/bec/warnings", methods=["POST"])
def add_warning():
    data = request.json
    minutes = data.get("minutes_before")
    message = data.get("message", "")
    if minutes is None or not message:
        return jsonify({"error": "minutes_before and message required"}), 400

    bec = load_json(BEC_FILE, DEFAULT_BEC)
    warnings = bec.get("warnings", [])
    warnings.append({"minutes_before": int(minutes), "message": message})
    warnings.sort(key=lambda w: w["minutes_before"], reverse=True)
    bec["warnings"] = warnings
    save_json(BEC_FILE, bec)
    add_log(f"Warning {minutes}min adicionado", "INFO")
    return jsonify(bec)


@app.route("/api/bec/warnings/<int:index>", methods=["DELETE"])
def remove_warning(index):
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    warnings = bec.get("warnings", [])
    if 0 <= index < len(warnings):
        removed = warnings.pop(index)
        bec["warnings"] = warnings
        save_json(BEC_FILE, bec)
        add_log(f"Warning {removed['minutes_before']}min removido", "INFO")
    return jsonify(bec)


@app.route("/api/bec/warnings/<int:index>", methods=["PUT"])
def update_warning(index):
    data = request.json
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    warnings = bec.get("warnings", [])
    if 0 <= index < len(warnings):
        if "minutes_before" in data:
            warnings[index]["minutes_before"] = int(data["minutes_before"])
        if "message" in data:
            warnings[index]["message"] = data["message"]
        warnings.sort(key=lambda w: w["minutes_before"], reverse=True)
        bec["warnings"] = warnings
        save_json(BEC_FILE, bec)
    return jsonify(bec)


@app.route("/api/bec/custom-jobs", methods=["POST"])
def add_custom_job():
    data = request.json
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    jobs = bec.get("custom_jobs", [])
    new_job = {
        "time": data.get("time", "00:00:00"),
        "delay": data.get("delay", "000000"),
        "day": data.get("day", "1,2,3,4,5,6,7"),
        "loop": data.get("loop", 0),
        "cmd": data.get("cmd", ""),
        "cmdtype": data.get("cmdtype", 0),
        "label": data.get("label", "Custom Job"),
    }
    jobs.append(new_job)
    bec["custom_jobs"] = jobs
    save_json(BEC_FILE, bec)
    add_log(f"Custom job '{new_job['label']}' adicionado", "INFO")
    return jsonify(bec)


@app.route("/api/bec/custom-jobs/<int:index>", methods=["DELETE"])
def remove_custom_job(index):
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    jobs = bec.get("custom_jobs", [])
    if 0 <= index < len(jobs):
        removed = jobs.pop(index)
        bec["custom_jobs"] = jobs
        save_json(BEC_FILE, bec)
        add_log(f"Custom job '{removed.get('label', '')}' removido", "INFO")
    return jsonify(bec)


@app.route("/api/bec/preview", methods=["GET"])
def preview_bec_xml():
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    xml_content = generate_bec_xml(bec)
    return xml_content, 200, {"Content-Type": "application/xml; charset=utf-8"}


@app.route("/api/bec/export", methods=["GET"])
def export_bec_xml():
    bec = load_json(BEC_FILE, DEFAULT_BEC)
    xml_content = generate_bec_xml(bec)
    buffer = BytesIO(xml_content.encode("utf-8"))
    buffer.seek(0)
    return send_file(
        buffer,
        mimetype="application/xml",
        as_attachment=True,
        download_name="scheduler.xml",
    )


if __name__ == "__main__":
    add_log("Gerenciador Fazcode initialized", "INFO")
    port = 5000
    print(f"\n  Gerenciador Fazcode running at http://localhost:{port}\n")

    if getattr(sys, "frozen", False):
        threading.Timer(1.5, lambda: webbrowser.open(f"http://localhost:{port}")).start()
        app.run(host="0.0.0.0", port=port, debug=False)
    else:
        app.run(host="0.0.0.0", port=port, debug=True)
