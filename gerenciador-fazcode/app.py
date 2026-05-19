"""
Gerenciador Fazcode - Process Manager & Restart on Crash
Dark/Pink Neon themed process monitoring and auto-restart tool.
"""
import json
import os
import signal
import subprocess
import threading
import time
from datetime import datetime
from pathlib import Path

import psutil
from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

DATA_DIR = Path(__file__).parent / "data"
DATA_DIR.mkdir(exist_ok=True)
SETTINGS_FILE = DATA_DIR / "settings.json"
PROCESSES_FILE = DATA_DIR / "processes.json"
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


if __name__ == "__main__":
    add_log("Gerenciador Fazcode initialized", "INFO")
    print("\n  🚀 Gerenciador Fazcode running at http://localhost:5000\n")
    app.run(host="0.0.0.0", port=5000, debug=True)
