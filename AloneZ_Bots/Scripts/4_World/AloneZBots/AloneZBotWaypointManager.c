// ============================================================================
// AloneZ Bots — Gerenciamento de Waypoints e Rotas
// ============================================================================

class AloneZBotWaypointManager
{
    protected ref AloneZRouteConfig m_RouteConfig;
    protected int m_CurrentIndex;
    protected int m_Direction; // 1 = forward, -1 = backward (para ALTERNATE)
    protected int m_CycleCount;
    protected bool m_RouteComplete;

    void AloneZBotWaypointManager(AloneZRouteConfig routeConfig)
    {
        m_RouteConfig = routeConfig;
        m_CurrentIndex = 0;
        m_Direction = 1;
        m_CycleCount = 0;
        m_RouteComplete = false;
    }

    // Retorna posicao do waypoint atual
    vector GetCurrentWaypoint()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return "0 0 0";

        if (m_CurrentIndex >= m_RouteConfig.Waypoints.Count())
            m_CurrentIndex = 0;

        return m_RouteConfig.Waypoints[m_CurrentIndex].Position;
    }

    // Retorna nome do waypoint atual
    string GetCurrentWaypointName()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return "Unknown";

        if (m_CurrentIndex >= m_RouteConfig.Waypoints.Count())
            m_CurrentIndex = 0;

        return m_RouteConfig.Waypoints[m_CurrentIndex].Name;
    }

    // Retorna indice atual
    int GetCurrentIndex()
    {
        return m_CurrentIndex;
    }

    // Retorna tempo de espera do waypoint atual
    float GetCurrentWaitTime()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return 5.0;

        if (m_CurrentIndex >= m_RouteConfig.Waypoints.Count())
            m_CurrentIndex = 0;

        return m_RouteConfig.Waypoints[m_CurrentIndex].WaitTime;
    }

    // Retorna velocidade do waypoint atual
    string GetCurrentSpeed()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return m_RouteConfig.DefaultSpeed;

        if (m_CurrentIndex >= m_RouteConfig.Waypoints.Count())
            m_CurrentIndex = 0;

        string speed = m_RouteConfig.Waypoints[m_CurrentIndex].Speed;
        if (speed == "")
            return m_RouteConfig.DefaultSpeed;

        return speed;
    }

    // Retorna stance do waypoint atual
    string GetCurrentStance()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return m_RouteConfig.DefaultStance;

        if (m_CurrentIndex >= m_RouteConfig.Waypoints.Count())
            m_CurrentIndex = 0;

        string stance = m_RouteConfig.Waypoints[m_CurrentIndex].Stance;
        if (stance == "")
            return m_RouteConfig.DefaultStance;

        return stance;
    }

    // Retorna acao do waypoint atual
    string GetCurrentAction()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return "NONE";

        if (m_CurrentIndex >= m_RouteConfig.Waypoints.Count())
            m_CurrentIndex = 0;

        return m_RouteConfig.Waypoints[m_CurrentIndex].Action;
    }

    // Retorna direcao de olhar do waypoint atual
    vector GetCurrentLookDirection()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return "0 0 1";

        if (m_CurrentIndex >= m_RouteConfig.Waypoints.Count())
            m_CurrentIndex = 0;

        return m_RouteConfig.Waypoints[m_CurrentIndex].LookDirection;
    }

    // Avanca para o proximo waypoint
    void AdvanceToNext()
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints || m_RouteConfig.Waypoints.Count() == 0)
            return;

        int waypointCount = m_RouteConfig.Waypoints.Count();

        switch (m_RouteConfig.WaypointBehavior)
        {
            case "LOOP":
                m_CurrentIndex++;
                if (m_CurrentIndex >= waypointCount)
                {
                    m_CurrentIndex = 0;
                    m_CycleCount++;
                    m_RouteComplete = true;
                }
                else
                {
                    m_RouteComplete = false;
                }
                break;

            case "ALTERNATE":
                m_CurrentIndex += m_Direction;
                if (m_CurrentIndex >= waypointCount)
                {
                    m_CurrentIndex = waypointCount - 2;
                    m_Direction = -1;
                    m_CycleCount++;
                    m_RouteComplete = true;
                }
                else if (m_CurrentIndex < 0)
                {
                    m_CurrentIndex = 1;
                    m_Direction = 1;
                    m_CycleCount++;
                    m_RouteComplete = true;
                }
                else
                {
                    m_RouteComplete = false;
                }
                break;

            case "ONCE":
                m_CurrentIndex++;
                if (m_CurrentIndex >= waypointCount)
                {
                    m_CurrentIndex = waypointCount - 1;
                    m_RouteComplete = true;
                    m_CycleCount = 1;
                }
                else
                {
                    m_RouteComplete = false;
                }
                break;

            case "RANDOM":
                int newIndex = m_CurrentIndex;
                if (waypointCount > 1)
                {
                    while (newIndex == m_CurrentIndex)
                    {
                        newIndex = Math.RandomInt(0, waypointCount);
                    }
                }
                m_CurrentIndex = newIndex;
                m_RouteComplete = false;
                break;

            default:
                // Fallback to LOOP
                m_CurrentIndex++;
                if (m_CurrentIndex >= waypointCount)
                {
                    m_CurrentIndex = 0;
                    m_CycleCount++;
                    m_RouteComplete = true;
                }
                break;
        }
    }

    // Verifica se a rota foi completada (um ciclo)
    bool IsRouteComplete()
    {
        return m_RouteComplete;
    }

    // Retorna quantidade de ciclos completos
    int GetCycleCount()
    {
        return m_CycleCount;
    }

    // Retorna total de waypoints
    int GetWaypointCount()
    {
        if (m_RouteConfig && m_RouteConfig.Waypoints)
            return m_RouteConfig.Waypoints.Count();
        return 0;
    }

    // Reseta para o inicio da rota
    void Reset()
    {
        m_CurrentIndex = 0;
        m_Direction = 1;
        m_CycleCount = 0;
        m_RouteComplete = false;
    }

    // Pega waypoint por indice especifico
    vector GetWaypointAt(int index)
    {
        if (!m_RouteConfig || !m_RouteConfig.Waypoints)
            return "0 0 0";

        if (index >= 0 && index < m_RouteConfig.Waypoints.Count())
            return m_RouteConfig.Waypoints[index].Position;

        return "0 0 0";
    }

    // Calcula distancia total da rota
    float GetTotalRouteDistance()
    {
        float totalDist = 0;
        if (!m_RouteConfig || !m_RouteConfig.Waypoints)
            return 0;

        for (int i = 0; i < m_RouteConfig.Waypoints.Count() - 1; i++)
        {
            vector wpA = m_RouteConfig.Waypoints[i].Position;
            vector wpB = m_RouteConfig.Waypoints[i + 1].Position;
            totalDist += vector.Distance(wpA, wpB);
        }

        // Distancia do ultimo ao primeiro (loop)
        if (m_RouteConfig.WaypointBehavior == "LOOP" && m_RouteConfig.Waypoints.Count() > 1)
        {
            int lastIdx = m_RouteConfig.Waypoints.Count() - 1;
            vector lastWp = m_RouteConfig.Waypoints[lastIdx].Position;
            vector firstWp = m_RouteConfig.Waypoints[0].Position;
            totalDist += vector.Distance(lastWp, firstWp);
        }

        return totalDist;
    }
}
