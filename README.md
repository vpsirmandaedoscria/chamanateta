# AloneZ BOTS - Sistema de AI Bots para DayZ

Sistema completo de bots AI para DayZ **sem dependência do DayZ Expansion**. Bots com patrulha automática, detecção de players, combate (ranged + faca), modo furtivo, e sistema de loot configurável por dificuldade.

## Funcionalidades

- **Spawn configurável** - Pontos de spawn em profiles JSON com posição, grupo, dificuldade
- **Log completo** - Logs de spawn, inicialização, posição X/Y/Z, combate, detecção, patrulha, morte, loot
- **Detecção de player** - Raio configurável (padrão 100m) com line-of-sight
- **Modo furtivo** - Bots se aproximam agachados antes de engajar
- **Combate ranged** - Disparo automático com precisão e spread configuráveis
- **Combate melee (faca)** - Ataque corpo-a-corpo quando próximo do player
- **Dificuldade individual** - Easy, Medium, Hard com configs separadas
- **Grupos de bots** - Grupos configuráveis por spawn com alerta coletivo
- **Rotas automáticas** - Waypoints em ciclo loop pelo mapa
- **Loot configurável** - Tabelas de loot separadas por dificuldade
- **Respawn** - Sistema de respawn automático com cooldown configurável

## Estrutura do Mod

```
AloneZ/BOTS/
├── config.cpp                          # Configuração do mod DayZ
├── Scripts/
│   ├── 3_Game/AloneZ_BOTS/
│   │   ├── ABLogger.c                  # Sistema de logging
│   │   └── Config/
│   │       ├── ABConfig.c              # Carregador de configurações
│   │       ├── ABDifficultyConfig.c    # Configuração de dificuldade
│   │       ├── ABSpawnConfig.c         # Configuração de pontos de spawn
│   │       └── ABLootConfig.c          # Configuração de tabelas de loot
│   ├── 4_World/AloneZ_BOTS/
│   │   ├── ABBot.c                     # Entidade principal do bot
│   │   ├── ABBotBrain.c                # Cérebro/lógica AI (FSM)
│   │   ├── ABBotDetection.c            # Sistema de detecção de players
│   │   ├── ABBotCombat.c               # Sistema de combate (ranged + melee)
│   │   ├── ABBotPatrol.c               # Sistema de patrulha/rotas
│   │   ├── ABBotGroup.c                # Gerenciamento de grupos
│   │   ├── ABBotLoot.c                 # Sistema de loot e equipamento
│   │   ├── ABSpawnManager.c            # Gerenciador de spawns
│   │   └── ABBotManager.c             # Gerenciador global de bots
│   └── 5_Mission/AloneZ_BOTS/
│       └── MissionServer.c             # Integração com a mission do servidor
└── Profiles/AloneZ/BOTS/               # Arquivos de configuração JSON
    ├── BotSettings.json                # Configurações gerais
    ├── Difficulty/
    │   ├── Easy.json                   # Config de dificuldade fácil
    │   ├── Medium.json                 # Config de dificuldade média
    │   └── Hard.json                   # Config de dificuldade difícil
    ├── Spawns/
    │   └── SpawnPoints.json            # Pontos de spawn dos bots
    └── Loot/
        ├── LootEasy.json               # Loot drop para bots fáceis
        ├── LootMedium.json             # Loot drop para bots médios
        └── LootHard.json              # Loot drop para bots difíceis
```

## Instalação

1. Copie a pasta `AloneZ/` para a raiz do seu servidor DayZ
2. Copie os arquivos de `Profiles/AloneZ/BOTS/` para `$profile/AloneZ/BOTS/` no seu servidor
3. Adicione `AloneZ_BOTS` na lista de mods do servidor
4. Reinicie o servidor

## Configuração

### BotSettings.json - Configurações Gerais

| Parâmetro | Tipo | Padrão | Descrição |
|-----------|------|--------|-----------|
| `Enabled` | bool | true | Habilita/desabilita o sistema de bots |
| `BotUpdateInterval` | float | 1.0 | Intervalo de atualização dos bots (segundos) |
| `PositionLogInterval` | float | 30.0 | Intervalo de log de posição (segundos) |
| `DetectionRadius` | float | 100.0 | Raio padrão de detecção (metros) |
| `StealthApproachDistance` | float | 50.0 | Distância para iniciar modo furtivo |
| `CombatEngageDistance` | float | 30.0 | Distância para iniciar combate ranged |
| `MeleeEngageDistance` | float | 3.0 | Distância para combate melee |
| `DespawnDistance` | float | 500.0 | Distância para despawn |
| `RespawnCooldown` | float | 300.0 | Cooldown de respawn (segundos) |
| `MaxBotsTotal` | int | 50 | Máximo de bots no servidor |
| `LogPositions` | bool | true | Logar posições dos bots |
| `LogCombat` | bool | true | Logar eventos de combate |
| `LogDetection` | bool | true | Logar detecções de players |
| `LogPatrol` | bool | true | Logar movimentação de patrulha |
| `LogSpawns` | bool | true | Logar spawns de bots |

### Dificuldade (Easy.json / Medium.json / Hard.json)

Cada dificuldade é configurada **individualmente** com os seguintes parâmetros:

| Parâmetro | Descrição | Easy | Medium | Hard |
|-----------|-----------|------|--------|------|
| `AccuracyMin` | Precisão mínima (0-1) | 0.1 | 0.3 | 0.6 |
| `AccuracyMax` | Precisão máxima (0-1) | 0.3 | 0.6 | 0.9 |
| `DamageMultiplier` | Multiplicador de dano | 0.5 | 1.0 | 1.5 |
| `DamageReceivedMultiplier` | Multiplicador de dano recebido | 1.5 | 1.0 | 0.7 |
| `ReactionTime` | Tempo de reação (seg) | 3.0 | 1.5 | 0.5 |
| `DetectionRadius` | Raio de detecção (m) | 60 | 100 | 150 |
| `MovementSpeedMultiplier` | Velocidade de movimento | 0.8 | 1.0 | 1.2 |
| `Brutality` | Chance de perseguir (0-1) | 0.2 | 0.5 | 0.9 |
| `MeleeChance` | Chance de usar faca (0-1) | 0.3 | 0.5 | 0.7 |
| `MeleeDamage` | Dano da faca | 15 | 25 | 40 |
| `FireRate` | Tiros por segundo | 0.5 | 1.0 | 2.0 |
| `AimSpread` | Dispersão do tiro (graus) | 8.0 | 5.0 | 2.0 |
| `HealthMultiplier` | Multiplicador de vida | 0.8 | 1.0 | 1.5 |
| `StealthDistance` | Distância furtiva (m) | 40 | 50 | 60 |
| `StealthSpeedMultiplier` | Velocidade furtiva | 0.5 | 0.6 | 0.8 |
| `DodgeChance` | Chance de esquiva (0-1) | 0.05 | 0.15 | 0.3 |
| `RespawnTime` | Tempo de respawn (seg) | 600 | 300 | 180 |

### SpawnPoints.json - Pontos de Spawn

Cada ponto de spawn configura um grupo de bots:

```json
{
    "Name": "Nome_Do_Spawn",
    "Enabled": true,
    "Position": [X, Y, Z],
    "Difficulty": "Easy|Medium|Hard",
    "GroupSize": 3,
    "GroupName": "Nome_Do_Grupo",
    "RespawnEnabled": true,
    "RespawnTime": 300.0,
    "SpawnChance": 1.0,
    "Loadout": "CivilianLoadout|MilitaryLoadout|HeavyMilitaryLoadout",
    "Waypoints": [
        { "Position": [X, Y, Z], "WaitTime": 5.0 },
        { "Position": [X, Y, Z], "WaitTime": 5.0 }
    ]
}
```

### Loot Tables (LootEasy.json / LootMedium.json / LootHard.json)

Cada item de loot é configurável:

```json
{
    "ClassName": "NomeDoItem",
    "QuantityMin": 1,
    "QuantityMax": 3,
    "DropChance": 0.5
}
```

## Sistema de Comportamento (FSM)

O bot segue um fluxo de estados:

```
IDLE → PATROLLING → DETECTING → STEALTH → COMBAT_RANGED / COMBAT_MELEE
  ↑                                             |
  └─────────────── (alvo perdido) ──────────────┘
```

1. **IDLE** - Bot recém-spawnado, aguarda 2s antes de patrulhar
2. **PATROLLING** - Segue waypoints em loop, escaneando por players
3. **DETECTING** - Player detectado, tempo de reação baseado na dificuldade
4. **STEALTH** - Aproximação furtiva (agachado, velocidade reduzida)
5. **COMBAT_RANGED** - Disparo automático com precisão configurável
6. **COMBAT_MELEE** - Ataque com faca quando próximo

## Logs

Os logs ficam em `$profile/AloneZ/BOTS/Logs/` com o formato:

```
[2026-05-07 12:00:00] [INFO] [SPAWN] Bot 'ABBot_1' spawned | Dificuldade: Medium | Posicao: X:6824 Y:15 Z:2480 | Grupo: Cherno_Squad
[2026-05-07 12:00:01] [INFO] [DETECTION] Bot 'ABBot_1' detectou player 'PlayerName' a 85.5m
[2026-05-07 12:00:02] [INFO] [STEALTH] Bot 'ABBot_1' em modo furtivo seguindo 'PlayerName'
[2026-05-07 12:00:05] [INFO] [COMBAT] Bot 'ABBot_1' disparou contra 'PlayerName' | HIT (Dano: 18.5)
[2026-05-07 12:00:10] [INFO] [DEATH] Bot 'ABBot_1' morreu | Morto por: PlayerName | Posicao: X:6830 Y:15 Z:2485
[2026-05-07 12:00:10] [INFO] [LOOT] Bot 'ABBot_1' dropou loot: AKM x1
```

## Dependências

- DayZ Standalone Server
- **NÃO requer DayZ Expansion**

## Licença

Uso livre para servidores DayZ.
