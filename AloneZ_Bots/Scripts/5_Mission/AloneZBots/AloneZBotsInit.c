// ============================================================================
// AloneZ Bots — Inicializacao do Mod no MissionServer
// Carrega configs, inicia spawner, gerencia ciclo de vida
// ============================================================================

modded class MissionServer
{
    ref AloneZBotSpawner m_AloneZSpawner;

    override void OnInit()
    {
        super.OnInit();

        // Log de inicio do mod
        AloneZBotsLogger.LogInfo("MOD_START", "AloneZ Bots v" + ALONEZ_VERSION + " iniciando...");

        // Carrega configuracoes
        AloneZBotsConfig.Load();

        if (!AloneZBotsConfig.IsEnabled())
        {
            AloneZBotsLogger.LogWarning("MOD_START", "Mod desabilitado via config. Nenhum bot sera spawnado.");
            return;
        }

        // Inicia o spawner
        m_AloneZSpawner = new AloneZBotSpawner();
        m_AloneZSpawner.Initialize();

        int routeCount = m_AloneZSpawner.GetRouteCount();
        int botCount = m_AloneZSpawner.GetTotalBotCount();

        string initMsg = "AloneZ Bots v" + ALONEZ_VERSION + " iniciado com sucesso. " + routeCount.ToString() + " rotas carregadas, " + botCount.ToString() + " bots configurados.";
        AloneZBotsLogger.LogInfo("MOD_START", initMsg);
    }

    override void OnMissionFinish()
    {
        if (m_AloneZSpawner)
        {
            m_AloneZSpawner.Cleanup();
            AloneZBotsLogger.LogInfo("MOD_STOP", "AloneZ Bots v" + ALONEZ_VERSION + " encerrado. Todos os bots removidos.");
        }

        super.OnMissionFinish();
    }
}
