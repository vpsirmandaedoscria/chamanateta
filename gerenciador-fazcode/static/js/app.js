/**
 * Gerenciador Fazcode - Frontend Application
 * Dark/Pink Neon Process Manager
 */

const API = {
  async get(url) {
    const res = await fetch(url);
    return res.json();
  },
  async post(url, data = {}) {
    const res = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data),
    });
    return res.json();
  },
  async put(url, data = {}) {
    const res = await fetch(url, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data),
    });
    return res.json();
  },
  async del(url) {
    const res = await fetch(url, { method: 'DELETE' });
    return res.json();
  },
};

// State
let processes = [];
let settings = {};
let monitoringActive = false;
let refreshInterval = null;
let editingProcessId = null;

// DOM elements
const $ = (sel) => document.querySelector(sel);
const $$ = (sel) => document.querySelectorAll(sel);

// Initialize
document.addEventListener('DOMContentLoaded', () => {
  initTabs();
  loadAll();
  startAutoRefresh();
});

function initTabs() {
  $$('.tab').forEach((tab) => {
    tab.addEventListener('click', () => {
      $$('.tab').forEach((t) => t.classList.remove('active'));
      $$('.tab-content').forEach((c) => c.classList.remove('active'));
      tab.classList.add('active');
      $(`#${tab.dataset.tab}`).classList.add('active');

      if (tab.dataset.tab === 'logs') loadLogs();
      if (tab.dataset.tab === 'bec') loadBec();
      if (tab.dataset.tab === 'rcon') loadRconConfig();
      if (tab.dataset.tab === 'bans') loadBans();
      if (tab.dataset.tab === 'players') { refreshPlayers(); loadPlayersHistory(); }
    });
  });
}

async function loadAll() {
  await Promise.all([loadProcesses(), loadSettings(), loadMonitoringStatus(), loadBec()]);
}

// ===== Processes =====
async function loadProcesses() {
  try {
    processes = await API.get('/api/processes');
    renderProcesses();
    updateStats();
  } catch (e) {
    console.error('Failed to load processes:', e);
  }
}

function renderProcesses() {
  const list = $('#process-list');
  const search = $('#search-input')?.value?.toLowerCase() || '';

  const filtered = processes.filter(
    (p) =>
      p.name.toLowerCase().includes(search) ||
      (p.process_name && p.process_name.toLowerCase().includes(search))
  );

  if (filtered.length === 0) {
    list.innerHTML = `
      <div class="empty-state">
        <div class="empty-state-icon">&#9881;</div>
        <div class="empty-state-text">Nenhum processo monitorado</div>
        <div class="empty-state-sub">Clique em "Adicionar" para monitorar um processo</div>
      </div>`;
    return;
  }

  list.innerHTML = filtered
    .map(
      (p) => `
    <div class="process-item" data-id="${p.id}">
      <div class="process-status-indicator ${p.status || 'unknown'}"></div>
      <div class="process-info">
        <div class="process-name">${escHtml(p.name)}</div>
        <div class="process-detail">
          ${p.process_name ? `Processo: ${escHtml(p.process_name)}` : ''}
          ${p.command ? ` &mdash; ${escHtml(p.command.substring(0, 60))}${p.command.length > 60 ? '...' : ''}` : ''}
        </div>
      </div>
      <div class="process-meta">
        <div class="process-meta-label">Restarts</div>
        <div class="process-meta-value">${p.restart_count || 0}</div>
      </div>
      <div class="process-actions">
        <button class="btn-icon" onclick="toggleProcess(${p.id}, ${!p.enabled})" title="${p.enabled ? 'Desativar' : 'Ativar'}">
          ${p.enabled ? '&#9210;' : '&#9211;'}
        </button>
        <button class="btn-icon" onclick="manualRestart(${p.id})" title="Reiniciar">&#8635;</button>
        <button class="btn-icon" onclick="killProcess(${p.id})" title="Parar">&#9632;</button>
        <button class="btn-icon" onclick="editProcess(${p.id})" title="Editar">&#9998;</button>
        <button class="btn-icon" onclick="deleteProcess(${p.id})" title="Remover">&#10005;</button>
      </div>
    </div>`
    )
    .join('');
}

function updateStats() {
  const total = processes.length;
  const running = processes.filter((p) => p.status === 'running').length;
  const stopped = processes.filter((p) => p.status === 'stopped').length;
  const restarts = processes.reduce((sum, p) => sum + (p.restart_count || 0), 0);

  $('#stat-total').textContent = total;
  $('#stat-running').textContent = running;
  $('#stat-stopped').textContent = stopped;
  $('#stat-restarts').textContent = restarts;
}

async function addProcess(data) {
  try {
    await API.post('/api/processes', data);
    await loadProcesses();
    showToast('Processo adicionado com sucesso', 'success');
    closeModal();
  } catch (e) {
    showToast('Erro ao adicionar processo', 'error');
  }
}

async function updateProcess(id, data) {
  try {
    await API.put(`/api/processes/${id}`, data);
    await loadProcesses();
    showToast('Processo atualizado', 'success');
    closeModal();
  } catch (e) {
    showToast('Erro ao atualizar processo', 'error');
  }
}

async function deleteProcess(id) {
  if (!confirm('Remover este processo da lista de monitoramento?')) return;
  try {
    await API.del(`/api/processes/${id}`);
    await loadProcesses();
    showToast('Processo removido', 'info');
  } catch (e) {
    showToast('Erro ao remover processo', 'error');
  }
}

async function manualRestart(id) {
  try {
    const res = await API.post(`/api/processes/${id}/restart`);
    if (res.success) {
      showToast('Processo reiniciado', 'success');
    } else {
      showToast('Falha ao reiniciar processo', 'error');
    }
    await loadProcesses();
  } catch (e) {
    showToast('Erro ao reiniciar', 'error');
  }
}

async function killProcess(id) {
  try {
    const res = await API.post(`/api/processes/${id}/kill`);
    if (res.success) {
      showToast('Processo encerrado', 'warning');
    } else {
      showToast(res.error || 'Processo n\u00e3o encontrado', 'error');
    }
    await loadProcesses();
  } catch (e) {
    showToast('Erro ao encerrar processo', 'error');
  }
}

async function toggleProcess(id, enabled) {
  await API.put(`/api/processes/${id}`, { enabled });
  await loadProcesses();
  showToast(enabled ? 'Monitoramento ativado' : 'Monitoramento desativado', 'info');
}

// ===== Monitoring =====
async function loadMonitoringStatus() {
  try {
    const data = await API.get('/api/monitoring/status');
    monitoringActive = data.active;
    updateMonitoringUI();
  } catch (e) {
    console.error('Failed to load monitoring status:', e);
  }
}

function updateMonitoringUI() {
  const statusEl = $('#monitor-status');
  const btnStart = $('#btn-start-monitor');
  const btnStop = $('#btn-stop-monitor');

  if (monitoringActive) {
    statusEl.className = 'monitor-status active';
    statusEl.innerHTML = '<span class="status-dot"></span> MONITORANDO';
    btnStart.style.display = 'none';
    btnStop.style.display = 'inline-flex';
  } else {
    statusEl.className = 'monitor-status inactive';
    statusEl.innerHTML = '<span class="status-dot"></span> PARADO';
    btnStart.style.display = 'inline-flex';
    btnStop.style.display = 'none';
  }
}

async function startMonitoring() {
  await API.post('/api/monitoring/start');
  monitoringActive = true;
  updateMonitoringUI();
  showToast('Monitoramento iniciado', 'success');
}

async function stopMonitoring() {
  await API.post('/api/monitoring/stop');
  monitoringActive = false;
  updateMonitoringUI();
  showToast('Monitoramento parado', 'warning');
}

// ===== Settings =====
async function loadSettings() {
  try {
    settings = await API.get('/api/settings');
    renderSettings();
  } catch (e) {
    console.error('Failed to load settings:', e);
  }
}

function renderSettings() {
  const el = $('#settings-check-interval');
  if (el) el.value = settings.check_interval || 5;

  const maxRestart = $('#settings-max-restarts');
  if (maxRestart) maxRestart.value = settings.max_restart_attempts || 10;

  const cooldown = $('#settings-cooldown');
  if (cooldown) cooldown.value = settings.restart_cooldown || 3;

  const maxLog = $('#settings-max-log');
  if (maxLog) maxLog.value = settings.max_log_lines || 500;

  const autoStart = $('#settings-auto-start');
  if (autoStart) autoStart.checked = settings.auto_start_monitoring || false;

  const sound = $('#settings-sound');
  if (sound) sound.checked = settings.sound_on_restart || true;
}

async function saveSettings() {
  const data = {
    check_interval: parseInt($('#settings-check-interval').value) || 5,
    max_restart_attempts: parseInt($('#settings-max-restarts').value) || 10,
    restart_cooldown: parseInt($('#settings-cooldown').value) || 3,
    max_log_lines: parseInt($('#settings-max-log').value) || 500,
    auto_start_monitoring: $('#settings-auto-start').checked,
    sound_on_restart: $('#settings-sound').checked,
  };

  try {
    await API.put('/api/settings', data);
    settings = data;
    showToast('Configura\u00e7\u00f5es salvas', 'success');
  } catch (e) {
    showToast('Erro ao salvar configura\u00e7\u00f5es', 'error');
  }
}

// ===== Logs =====
async function loadLogs() {
  try {
    const data = await API.get('/api/logs?limit=200');
    renderLogs(data.logs);
  } catch (e) {
    console.error('Failed to load logs:', e);
  }
}

function renderLogs(logs) {
  const container = $('#log-content');
  if (!logs || logs.length === 0) {
    container.innerHTML = '<div class="log-empty">Nenhum log registrado ainda</div>';
    return;
  }

  container.innerHTML = logs
    .reverse()
    .map((line) => {
      const match = line.match(/\[(.+?)\] \[(.+?)\] (.+)/);
      if (match) {
        return `<div class="log-line">
          <span class="timestamp">${escHtml(match[1])}</span>
          <span class="level-${match[2]}">[${match[2]}]</span>
          ${escHtml(match[3])}
        </div>`;
      }
      return `<div class="log-line">${escHtml(line)}</div>`;
    })
    .join('');

  container.scrollTop = 0;
}

async function clearLogs() {
  if (!confirm('Limpar todos os logs?')) return;
  await API.del('/api/logs');
  loadLogs();
  showToast('Logs limpos', 'info');
}

// ===== BEC Scheduler =====
let becData = {};

async function loadBec() {
  try {
    becData = await API.get('/api/bec');
    renderBec();
  } catch (e) {
    console.error('Failed to load BEC data:', e);
  }
}

function renderBec() {
  renderBecTimes();
  renderBecDays();
  renderBecWarnings();
  renderBecKick();
  renderBecLock();
  renderBecCustomJobs();
}

function renderBecTimes() {
  const container = $('#bec-times-list');
  if (!container) return;
  const times = becData.restart_times || [];
  if (times.length === 0) {
    container.innerHTML = '<div class="bec-empty">Nenhum horário configurado</div>';
    return;
  }
  container.innerHTML = times.map(t => `
    <div class="bec-time-item">
      <span class="bec-time-value">${t}</span>
      <button class="btn-icon btn-sm" onclick="removeRestartTime('${t}')" title="Remover">&#10005;</button>
    </div>
  `).join('');
}

function renderBecDays() {
  const days = becData.days || [1,2,3,4,5,6,7];
  for (let i = 1; i <= 7; i++) {
    const cb = $(`#bec-day-${i}`);
    if (cb) cb.checked = days.includes(i);
  }
}

function renderBecWarnings() {
  const container = $('#bec-warnings-list');
  if (!container) return;
  const warnings = becData.warnings || [];
  if (warnings.length === 0) {
    container.innerHTML = '<div class="bec-empty">Nenhum aviso configurado</div>';
    return;
  }
  container.innerHTML = warnings.map((w, i) => `
    <div class="bec-warning-item">
      <span class="bec-warn-time">${w.minutes_before} min</span>
      <input type="text" class="form-input bec-warn-msg-input" value="${escHtml(w.message)}" 
        onchange="updateWarning(${i}, this.value)">
      <button class="btn-icon btn-sm" onclick="removeWarning(${i})" title="Remover">&#10005;</button>
    </div>
  `).join('');
}

function renderBecKick() {
  const en = $('#bec-kick-enabled');
  if (en) en.checked = becData.kick_before_restart !== false;
  const min = $('#bec-kick-minutes');
  if (min) min.value = becData.kick_minutes_before || 1;
  const msg = $('#bec-kick-message');
  if (msg) msg.value = becData.kick_message || '';
}

function renderBecLock() {
  const en = $('#bec-lock-enabled');
  if (en) en.checked = becData.lock_before_restart !== false;
  const min = $('#bec-lock-minutes');
  if (min) min.value = becData.lock_minutes_before || 2;
  const cmd = $('#bec-shutdown-cmd');
  if (cmd) cmd.value = becData.shutdown_command || '#shutdown';
}

function renderBecCustomJobs() {
  const container = $('#bec-custom-jobs-list');
  if (!container) return;
  const jobs = becData.custom_jobs || [];
  if (jobs.length === 0) {
    container.innerHTML = '<div class="bec-empty">Nenhum job customizado</div>';
    return;
  }
  container.innerHTML = jobs.map((j, i) => `
    <div class="bec-custom-job-item">
      <span class="bec-cj-label">${escHtml(j.label || 'Job ' + i)}</span>
      <span class="bec-cj-time">${j.time || '00:00:00'}</span>
      <span class="bec-cj-cmd">${escHtml(j.cmd)}</span>
      <button class="btn-icon btn-sm" onclick="removeCustomJob(${i})" title="Remover">&#10005;</button>
    </div>
  `).join('');
}

async function addRestartTime() {
  const input = $('#bec-new-time');
  const time = input?.value;
  if (!time) { showToast('Selecione um horário', 'error'); return; }
  becData = await API.post('/api/bec/restart-times', { time });
  renderBec();
  input.value = '';
  showToast(`Horário ${time} adicionado`, 'success');
}

async function removeRestartTime(time) {
  const res = await fetch('/api/bec/restart-times', {
    method: 'DELETE',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ time }),
  });
  becData = await res.json();
  renderBec();
  showToast(`Horário ${time} removido`, 'info');
}

async function addWarning() {
  const minInput = $('#bec-warn-minutes');
  const msgInput = $('#bec-warn-message');
  const minutes = parseInt(minInput?.value);
  const message = msgInput?.value?.trim();
  if (!minutes || !message) { showToast('Preencha minutos e mensagem', 'error'); return; }
  becData = await API.post('/api/bec/warnings', { minutes_before: minutes, message });
  renderBec();
  minInput.value = '';
  msgInput.value = '';
  showToast('Aviso adicionado', 'success');
}

async function removeWarning(index) {
  const res = await fetch(`/api/bec/warnings/${index}`, { method: 'DELETE' });
  becData = await res.json();
  renderBec();
  showToast('Aviso removido', 'info');
}

async function updateWarning(index, message) {
  becData = await API.put(`/api/bec/warnings/${index}`, { message });
  showToast('Aviso atualizado', 'success');
}

async function addCustomJob() {
  const label = $('#bec-cj-label')?.value?.trim();
  const time = $('#bec-cj-time')?.value;
  const cmd = $('#bec-cj-cmd')?.value?.trim();
  if (!cmd) { showToast('Comando é obrigatório', 'error'); return; }
  const timeFormatted = time ? time + (time.split(':').length < 3 ? ':00' : '') : '00:00:00';
  becData = await API.post('/api/bec/custom-jobs', {
    label: label || 'Custom Job',
    time: timeFormatted,
    cmd: cmd,
    day: '1,2,3,4,5,6,7',
    loop: 0,
    cmdtype: 0,
  });
  renderBec();
  $('#bec-cj-label').value = '';
  $('#bec-cj-time').value = '';
  $('#bec-cj-cmd').value = '';
  showToast('Job customizado adicionado', 'success');
}

async function removeCustomJob(index) {
  const res = await fetch(`/api/bec/custom-jobs/${index}`, { method: 'DELETE' });
  becData = await res.json();
  renderBec();
  showToast('Job removido', 'info');
}

async function saveBecSettings() {
  const days = [];
  for (let i = 1; i <= 7; i++) {
    if ($(`#bec-day-${i}`)?.checked) days.push(i);
  }
  const data = {
    days: days,
    kick_before_restart: $('#bec-kick-enabled')?.checked || false,
    kick_minutes_before: parseInt($('#bec-kick-minutes')?.value) || 1,
    kick_message: $('#bec-kick-message')?.value || '',
    lock_before_restart: $('#bec-lock-enabled')?.checked || false,
    lock_minutes_before: parseInt($('#bec-lock-minutes')?.value) || 2,
    shutdown_command: $('#bec-shutdown-cmd')?.value || '#shutdown',
  };
  becData = await API.put('/api/bec', data);
  showToast('Configurações BEC salvas', 'success');
}

async function previewXml() {
  const container = $('#bec-xml-preview');
  if (!container) return;
  try {
    const res = await fetch('/api/bec/preview');
    const xml = await res.text();
    container.textContent = xml;
    container.style.display = container.style.display === 'none' ? 'block' : 'none';
  } catch (e) {
    showToast('Erro ao carregar preview', 'error');
  }
}

function exportXml() {
  window.location.href = '/api/bec/export';
  showToast('Exportando scheduler.xml...', 'success');
}

// ===== RCON =====
let rconConnected = false;

async function loadRconConfig() {
  try {
    const data = await API.get('/api/rcon/config');
    $('#rcon-host').value = data.host || '127.0.0.1';
    $('#rcon-port').value = data.port || 2302;
    $('#rcon-kickban').checked = data.kickban_enabled !== false;
    rconConnected = data.connected || false;
    updateRconStatus();
    loadChatLog();
  } catch (e) { console.error('Failed to load RCON config:', e); }
}

function updateRconStatus() {
  const el = $('#rcon-status');
  if (!el) return;
  if (rconConnected) {
    el.className = 'rcon-status connected';
    el.innerHTML = '<span class="status-dot"></span> CONECTADO';
  } else {
    el.className = 'rcon-status disconnected';
    el.innerHTML = '<span class="status-dot"></span> DESCONECTADO';
  }
}

async function saveRconConfig() {
  const data = {
    host: $('#rcon-host')?.value || '127.0.0.1',
    port: parseInt($('#rcon-port')?.value) || 2302,
    password: $('#rcon-password')?.value || '',
    kickban_enabled: $('#rcon-kickban')?.checked || false,
  };
  await API.put('/api/rcon/config', data);
  showToast('Config RCON salva', 'success');
}

async function connectRcon() {
  await saveRconConfig();
  const res = await API.post('/api/rcon/connect');
  rconConnected = res.connected || false;
  updateRconStatus();
  if (rconConnected) {
    showToast('RCON conectado!', 'success');
  } else {
    showToast('Falha ao conectar RCON', 'error');
  }
}

async function disconnectRcon() {
  await API.post('/api/rcon/disconnect');
  rconConnected = false;
  updateRconStatus();
  showToast('RCON desconectado', 'info');
}

async function sendRconCmd() {
  const input = $('#rcon-cmd-input');
  const cmd = input?.value?.trim();
  if (!cmd) return;
  try {
    const res = await API.post('/api/rcon/command', { command: cmd });
    if (res.offline) { showToast('RCON não conectado', 'error'); return; }
    appendChat(res.time, `> ${cmd}`, 'cmd');
    if (res.response) appendChat(res.time, res.response, 'response');
    input.value = '';
  } catch (e) { showToast('Erro ao enviar comando', 'error'); }
}

async function sendRconSay() {
  const input = $('#rcon-cmd-input');
  const msg = input?.value?.trim();
  if (!msg) return;
  try {
    const res = await API.post('/api/rcon/say', { message: msg });
    if (res.offline) { showToast('RCON não conectado', 'error'); return; }
    appendChat(res.time, `[CHAT] ${msg}`, 'chat');
    input.value = '';
  } catch (e) { showToast('Erro ao enviar mensagem', 'error'); }
}

async function sendQuickCmd(cmd) {
  try {
    const res = await API.post('/api/rcon/command', { command: cmd });
    if (res.offline) { showToast('RCON não conectado', 'error'); return; }
    appendChat(res.time, `> ${cmd}`, 'cmd');
    if (res.response) appendChat(res.time, res.response, 'response');
    showToast(`Comando "${cmd}" enviado`, 'success');
  } catch (e) { showToast('Erro ao enviar comando', 'error'); }
}

function appendChat(time, text, type) {
  const log = $('#rcon-chat-log');
  if (!log) return;
  if (log.querySelector('.bec-empty')) log.innerHTML = '';
  const cls = type === 'cmd' ? 'rcon-chat-cmd' : type === 'chat' ? 'rcon-chat-say' : 'rcon-chat-resp';
  log.innerHTML += `<div class="rcon-chat-line ${cls}"><span class="rcon-chat-time">[${time}]</span> ${escHtml(text)}</div>`;
  log.scrollTop = log.scrollHeight;
}

async function loadChatLog() {
  try {
    const data = await API.get('/api/rcon/chat');
    const log = $('#rcon-chat-log');
    if (!log || !data.length) return;
    log.innerHTML = '';
    data.forEach(e => {
      const type = e.type === 'chat' ? 'chat' : (e.cmd ? 'cmd' : 'response');
      appendChat(e.time, e.cmd ? `> ${e.cmd}` : '', 'cmd');
      if (e.response) appendChat(e.time, e.response, 'response');
    });
  } catch (e) { /* ignore */ }
}

// ===== Bans =====
let allBans = [];

async function loadBans() {
  try {
    allBans = await API.get('/api/bans');
    renderBans();
  } catch (e) { console.error('Failed to load bans:', e); }
}

function renderBans(filter) {
  const container = $('#bans-list');
  if (!container) return;
  let bans = allBans;
  if (filter) {
    const f = filter.toLowerCase();
    bans = bans.filter(b => (b.steam_id || '').toLowerCase().includes(f) || (b.player_name || '').toLowerCase().includes(f));
  }
  if (bans.length === 0) {
    container.innerHTML = '<div class="bec-empty">Nenhum ban registrado</div>';
    return;
  }
  container.innerHTML = bans.map(b => `
    <div class="ban-item ${b.active ? 'active' : 'inactive'}">
      <div class="ban-info">
        <div class="ban-player">${escHtml(b.player_name || 'Desconhecido')}</div>
        <div class="ban-steam">${escHtml(b.steam_id)}</div>
        <div class="ban-reason">${escHtml(b.reason || 'Sem motivo')}</div>
      </div>
      <div class="ban-meta">
        <div class="ban-date">${b.banned_at || ''}</div>
        <div class="ban-duration">${b.duration === 0 ? 'PERMANENTE' : b.duration + ' min'}</div>
        <div class="ban-status-badge ${b.active ? 'ban-active' : 'ban-expired'}">${b.active ? 'ATIVO' : 'INATIVO'}</div>
      </div>
      <div class="ban-actions">
        ${b.active ? `<button class="btn btn-success btn-sm" onclick="unbanPlayer(${b.id})" title="Desbanir">Unban</button>` : ''}
        <button class="btn-icon btn-sm" onclick="deleteBan(${b.id})" title="Remover">&#10005;</button>
      </div>
    </div>
  `).join('');
}

function filterBans() {
  const search = $('#ban-search')?.value || '';
  renderBans(search);
}

function openBanModal() { $('#ban-modal').classList.add('active'); }
function closeBanModal() { $('#ban-modal').classList.remove('active'); }

async function saveBan() {
  const steamId = $('#ban-steam-id')?.value?.trim();
  if (!steamId) { showToast('Steam ID é obrigatório', 'error'); return; }
  await API.post('/api/bans', {
    steam_id: steamId,
    player_name: $('#ban-player-name')?.value?.trim() || '',
    reason: $('#ban-reason')?.value?.trim() || '',
    duration: parseInt($('#ban-duration')?.value) || 0,
  });
  closeBanModal();
  await loadBans();
  showToast('Ban adicionado', 'success');
}

async function unbanPlayer(id) {
  if (!confirm('Desbanir este player?')) return;
  await API.post(`/api/bans/${id}/unban`);
  await loadBans();
  showToast('Player desbanido', 'success');
}

async function deleteBan(id) {
  if (!confirm('Remover este ban do banco de dados?')) return;
  await API.del(`/api/bans/${id}`);
  await loadBans();
  showToast('Ban removido', 'info');
}

function exportBans() {
  window.location.href = '/api/bans/export';
  showToast('Exportando bans.txt...', 'success');
}

// ===== Players =====
let allPlayersHistory = [];

async function refreshPlayers() {
  const container = $('#players-online-list');
  if (!container) return;
  try {
    const data = await API.get('/api/rcon/players');
    if (data.offline) {
      container.innerHTML = '<div class="bec-empty">Conecte ao RCON para ver os players online</div>';
      return;
    }
    const players = data.players || [];
    if (players.length === 0) {
      container.innerHTML = '<div class="bec-empty">Nenhum player online (ou servidor vazio)</div>';
      return;
    }
    container.innerHTML = players.map(p => `
      <div class="player-item">
        <div class="player-id">#${p.id}</div>
        <div class="player-info">
          <div class="player-name">${escHtml(p.name)}</div>
          <div class="player-guid">${escHtml(p.guid || '')}</div>
        </div>
        <div class="player-actions">
          <button class="btn btn-secondary btn-sm" onclick="sendQuickCmd('kick ${p.id} Kicked by admin')">Kick</button>
          <button class="btn btn-danger btn-sm" onclick="banFromList('${p.id}', '${escHtml(p.name)}', '${escHtml(p.guid)}')">Ban</button>
        </div>
      </div>
    `).join('');
  } catch (e) {
    container.innerHTML = '<div class="bec-empty">Erro ao buscar players</div>';
  }
}

function banFromList(playerId, name, guid) {
  $('#ban-steam-id').value = guid || '';
  $('#ban-player-name').value = name || '';
  $('#ban-reason').value = '';
  $('#ban-duration').value = '0';
  openBanModal();
}

async function loadPlayersHistory() {
  try {
    allPlayersHistory = await API.get('/api/players/history');
    renderPlayersHistory();
  } catch (e) { console.error('Failed to load players history:', e); }
}

function renderPlayersHistory(filter) {
  const container = $('#players-history-list');
  if (!container) return;
  let players = allPlayersHistory;
  if (filter) {
    const f = filter.toLowerCase();
    players = players.filter(p => (p.steam_id || '').toLowerCase().includes(f) || (p.player_name || '').toLowerCase().includes(f));
  }
  if (players.length === 0) {
    container.innerHTML = '<div class="bec-empty">Nenhum player registrado</div>';
    return;
  }
  container.innerHTML = players.map(p => `
    <div class="player-item ${p.is_banned ? 'player-banned' : ''}">
      <div class="player-info">
        <div class="player-name">${escHtml(p.player_name || 'Desconhecido')} ${p.is_banned ? '<span class="ban-badge">BANIDO</span>' : ''}</div>
        <div class="player-steam">${escHtml(p.steam_id)}</div>
      </div>
      <div class="player-meta">
        <div>Visto: ${p.times_seen || 1}x</div>
        <div>Último: ${p.last_seen || '-'}</div>
      </div>
      <div class="player-actions">
        ${!p.is_banned ? `<button class="btn btn-danger btn-sm" onclick="banFromHistory('${escHtml(p.steam_id)}', '${escHtml(p.player_name)}')">Ban</button>` : ''}
        <button class="btn-icon btn-sm" onclick="deletePlayerHistory(${p.id})" title="Remover">&#10005;</button>
      </div>
    </div>
  `).join('');
}

function filterPlayers() {
  const search = $('#player-search')?.value || '';
  renderPlayersHistory(search);
}

function banFromHistory(steamId, name) {
  $('#ban-steam-id').value = steamId;
  $('#ban-player-name').value = name;
  $('#ban-reason').value = '';
  $('#ban-duration').value = '0';
  openBanModal();
}

function openPlayerModal() { $('#player-modal').classList.add('active'); }
function closePlayerModal() { $('#player-modal').classList.remove('active'); }

async function savePlayer() {
  const steamId = $('#player-steam-id')?.value?.trim();
  if (!steamId) { showToast('Steam ID é obrigatório', 'error'); return; }
  await API.post('/api/players/history', {
    steam_id: steamId,
    player_name: $('#player-name-input')?.value?.trim() || '',
  });
  closePlayerModal();
  await loadPlayersHistory();
  showToast('Player adicionado', 'success');
}

async function deletePlayerHistory(id) {
  if (!confirm('Remover este player do histórico?')) return;
  await API.del(`/api/players/history/${id}`);
  await loadPlayersHistory();
  showToast('Player removido', 'info');
}

// ===== Modal =====
function openAddModal() {
  editingProcessId = null;
  $('#modal-title').textContent = 'ADICIONAR PROCESSO';
  $('#form-name').value = '';
  $('#form-process-name').value = '';
  $('#form-command').value = '';
  $('#form-working-dir').value = '';
  $('#form-arguments').value = '';
  $('#form-enabled').checked = true;
  $('#form-auto-restart').checked = true;
  $('#modal-overlay').classList.add('active');
}

function editProcess(id) {
  const proc = processes.find((p) => p.id === id);
  if (!proc) return;

  editingProcessId = id;
  $('#modal-title').textContent = 'EDITAR PROCESSO';
  $('#form-name').value = proc.name || '';
  $('#form-process-name').value = proc.process_name || '';
  $('#form-command').value = proc.command || '';
  $('#form-working-dir').value = proc.working_dir || '';
  $('#form-arguments').value = proc.arguments || '';
  $('#form-enabled').checked = proc.enabled !== false;
  $('#form-auto-restart').checked = proc.auto_restart !== false;
  $('#modal-overlay').classList.add('active');
}

function closeModal() {
  $('#modal-overlay').classList.remove('active');
  editingProcessId = null;
}

function saveProcess() {
  const data = {
    name: $('#form-name').value.trim(),
    process_name: $('#form-process-name').value.trim(),
    command: $('#form-command').value.trim(),
    working_dir: $('#form-working-dir').value.trim(),
    arguments: $('#form-arguments').value.trim(),
    enabled: $('#form-enabled').checked,
    auto_restart: $('#form-auto-restart').checked,
  };

  if (!data.name) {
    showToast('Nome \u00e9 obrigat\u00f3rio', 'error');
    return;
  }

  if (editingProcessId) {
    updateProcess(editingProcessId, data);
  } else {
    addProcess(data);
  }
}

// ===== System Processes Browser =====
async function browseSystemProcesses() {
  const search = $('#sys-process-search')?.value || '';
  try {
    const procs = await API.get(`/api/system/processes?search=${encodeURIComponent(search)}`);
    const container = $('#sys-process-list');
    container.innerHTML = procs
      .map(
        (p) => `
      <div class="sys-process-item" onclick="selectSystemProcess('${escHtml(p.name)}', '${escHtml(p.exe)}')">
        <span class="proc-name">${escHtml(p.name)}</span>
        <span class="proc-mem">${p.memory_mb} MB</span>
      </div>`
      )
      .join('');
  } catch (e) {
    console.error('Failed to browse system processes:', e);
  }
}

function selectSystemProcess(name, exe) {
  $('#form-process-name').value = name;
  if (exe) $('#form-command').value = exe;
  $('#sys-process-modal').classList.remove('active');
}

function openSysProcessModal() {
  $('#sys-process-modal').classList.add('active');
  browseSystemProcesses();
}

function closeSysProcessModal() {
  $('#sys-process-modal').classList.remove('active');
}

// ===== Auto Refresh =====
function startAutoRefresh() {
  refreshInterval = setInterval(() => {
    loadProcesses();
    loadMonitoringStatus();
  }, 3000);
}

// ===== Toast Notifications =====
function showToast(message, type = 'info') {
  const container = $('#toast-container');
  const toast = document.createElement('div');
  toast.className = `toast ${type}`;

  const icons = {
    success: '&#10003;',
    error: '&#10007;',
    warning: '&#9888;',
    info: '&#8505;',
  };

  toast.innerHTML = `
    <span style="font-size:16px">${icons[type] || icons.info}</span>
    <span>${escHtml(message)}</span>
  `;

  container.appendChild(toast);
  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateX(100px)';
    toast.style.transition = 'all 0.3s';
    setTimeout(() => toast.remove(), 300);
  }, 3500);
}

// ===== Helpers =====
function escHtml(str) {
  if (!str) return '';
  const div = document.createElement('div');
  div.textContent = str;
  return div.innerHTML;
}
