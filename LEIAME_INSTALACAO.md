# AloneZ Bots — Guia de Instalacao e Configuracao

## Instalacao

### 1. Copiar o mod para o servidor

Copie a pasta `@AloneZ` para o diretorio raiz do seu servidor DayZ:

```
C:\DayZServer\@AloneZ\
├── Addons\
│   └── AloneZ_Bots.pbo    (voce precisa empacotar com DayZ Tools)
├── Keys\
│   └── AloneZ_Bots.bikey
└── mod.cpp
```

### 2. Empacotar o PBO

Use o **DayZ Tools** (disponivel no Steam) para empacotar a pasta `AloneZ_Bots/` em um arquivo `.pbo`:

1. Abra o **DayZ Tools** pelo Steam
2. Va em **Tools > Addon Builder**
3. Em "Source directory", selecione a pasta `AloneZ_Bots/` (a que contem config.cpp e Scripts/)
4. Em "Destination directory", selecione `@AloneZ/Addons/`
5. Clique em **Pack**

### 3. Adicionar ao launch parameter

No seu arquivo de inicializacao do servidor (`.bat` ou `.cfg`), adicione o mod:

```batch
-mod=@AloneZ
```

Exemplo completo:
```batch
start "DayZ Server" /min "DayZServer_x64.exe" -config=serverDZ.cfg -port=2302 -profiles=profiles -mod=@AloneZ -dologs -adminlog
```

### 4. Primeira execucao

Na primeira execucao, o mod vai criar automaticamente:

```
profiles\AloneZ\Bots\
├── Config\
│   ├── AloneZBotsSettings.json     (configuracoes gerais)
│   ├── Routes\
│   │   └── route_military_base.json (rota de exemplo)
│   └── Loadouts\
│       ├── loadout_soldier.json
│       ├── loadout_bandit.json
│       └── loadout_civilian.json
└── Logs\
    ├── AloneZ_Bots_2026-05-18.log  (log diario)
    └── AloneZ_Bots_Events.log      (eventos criticos)
```

Voce tambem pode copiar os arquivos da pasta `DefaultConfigs/` para `profiles\AloneZ\Bots\Config\` manualmente se preferir configurar antes de iniciar.

---

## Configuracao

### Configuracoes Gerais (AloneZBotsSettings.json)

| Parametro | Descricao | Padrao |
|---|---|---|
| `ModEnabled` | Liga/desliga o mod (1/0) | 1 |
| `DebugMode` | Ativa logs detalhados (1/0) | 0 |
| `MaxBotsTotal` | Maximo de bots simultaneos | 50 |
| `BotDetectionRange` | Distancia de deteccao de jogadores (metros) | 150.0 |
| `BotEngageRange` | Distancia para iniciar combate (metros) | 100.0 |
| `BotDisengageRange` | Distancia para parar combate (metros) | 200.0 |
| `BotRespawnEnabled` | Bots respawnam apos morrer (1/0) | 1 |
| `BotRespawnDelaySeconds` | Tempo de respawn (segundos) | 300 |

### Sistema de Accuracy (Precisao)

| Parametro | Descricao | Padrao |
|---|---|---|
| `GlobalMinAccuracy` | Precisao minima global (0.0-1.0) | 0.1 |
| `GlobalMaxAccuracy` | Precisao maxima global (0.0-1.0) | 0.9 |
| `AccuracyScaleWithDistance` | Accuracy reduz com distancia (1/0) | 1 |
| `HeadshotChance` | Chance de headshot (0.0-1.0) | 0.05 |
| `BurstFireEnabled` | Tiro em rajada (1/0) | 1 |
| `ReactionTimeMin/Max` | Tempo de reacao antes do primeiro tiro (segundos) | 0.5-2.0 |

**Valores de accuracy:**
- `0.1` = Pessima (civil)
- `0.3` = Baixa (bandido)
- `0.5` = Media (soldado)
- `0.7` = Boa (treinado)
- `0.9` = Excelente (sniper)

### Criar Nova Rota

1. Crie um arquivo `.json` em `profiles\AloneZ\Bots\Config\Routes\`
2. Use o modelo `route_military_base.json` como base
3. Modifique os waypoints com as coordenadas do mapa

**Como pegar coordenadas:**
- Use https://www.izurvive.com/ para pegar coordenadas do mapa
- No jogo como admin: pressione `F11` ou use `#position`
- As coordenadas sao no formato `[X, Y, Z]` onde Y e a altitude

### Criar Novo Loadout

1. Crie um arquivo `.json` em `profiles\AloneZ\Bots\Config\Loadouts\`
2. Use `loadout_soldier.json` como modelo
3. Altere os classnames das armas e roupas

### Formacoes Disponiveis

| Formacao | Descricao |
|---|---|
| `COLUMN` | Fila indiana |
| `LINE` | Linha horizontal |
| `WEDGE` | Formacao em V |
| `STAGGERED` | Zigue-zague |
| `RANDOM` | Aleatorio |

### Comportamentos de Rota

| Comportamento | Descricao |
|---|---|
| `LOOP` | Repete a rota em ciclo |
| `ALTERNATE` | Vai e volta |
| `ONCE` | Percorre uma vez e para |
| `RANDOM` | Waypoints aleatorios |

---

## Logs

Os logs ficam em `profiles\AloneZ\Bots\Logs\`

### Eventos registrados:

| Tag | Descricao |
|---|---|
| `[MOD_START]` | Mod iniciou |
| `[MOD_STOP]` | Mod encerrou |
| `[ROUTE_START]` | Rota iniciada com bots |
| `[ROUTE_END]` | Rota completou um ciclo |
| `[BOT_SPAWN]` | Bot spawnado |
| `[BOT_DEATH]` | Bot morreu (inclui info do jogador killer) |
| `[ROUTE_INTERCEPTED]` | Jogador detectado, rota interrompida |
| `[COMBAT_START]` | Grupo entrou em combate |
| `[COMBAT_END]` | Grupo saiu do combate |
| `[WAYPOINT]` | Bot alcancou waypoint |
| `[CONFIG_RELOAD]` | Configuracoes recarregadas |

### Exemplo de log:
```
[2026-05-18 03:10:00] [INFO] [MOD_START] AloneZ Bots v1.0.0 iniciado. 3 rotas carregadas, 9 bots configurados.
[2026-05-18 03:10:01] [INFO] [ROUTE_START] Rota "Patrulha Base Militar" (route_military_base) iniciada. 4 bots spawnados.
[2026-05-18 03:14:00] [WARNING] [ROUTE_INTERCEPTED] Rota "Patrulha Base Militar" interceptada! Bot "Mirek_0" detectou ameaca. Grupo entrando em combate.
[2026-05-18 03:15:30] [WARNING] [BOT_DEATH] Bot "Mirek_0" (route_military_base) morto por jogador "Player123" (SteamID: 76561198XXXXXXXX) na posicao <4550, 342, 8945>. Arma: M4A1. Distancia: 85.3m.
```

---

## Estrutura dos Scripts

```
AloneZ_Bots/Scripts/
├── 3_Game/AloneZBots/
│   ├── AloneZBotsConfig.c      — Carregamento de configs JSON
│   └── AloneZBotsLogger.c      — Sistema de logs
├── 4_World/AloneZBots/
│   ├── AloneZBotEntity.c       — Entidade do bot (herda SurvivorBase)
│   ├── AloneZBotBrain.c        — FSM (Patrol/Idle/Combat/Search/Flee/Dead)
│   ├── AloneZBotWaypointManager.c — Rotas e waypoints
│   ├── AloneZBotCombatHandler.c — Combate e accuracy
│   ├── AloneZBotAnimHandler.c  — Animacoes
│   ├── AloneZBotSpawner.c      — Spawner de bots
│   └── AloneZBotGroup.c        — Gerenciamento de grupo
└── 5_Mission/AloneZBots/
    └── AloneZBotsInit.c        — Inicializacao no servidor
```
