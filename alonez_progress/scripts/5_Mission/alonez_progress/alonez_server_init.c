modded class MissionServer
{
    protected float m_AloneZMinuteAcc = 0;
    protected float m_AloneZTickAcc = 0;
    protected ref map<string, vector> m_AloneZLastPos = new map<string, vector>();
    protected ref map<string, PlayerBase> m_AloneZSeenPB = new map<string, PlayerBase>();
    protected ref map<string, bool> m_AloneZSynced = new map<string, bool>();
    protected ref map<string, float>  m_AloneZDistAcc = new map<string, float>();
    protected ref map<string, int>    m_AloneZHourMin = new map<string, int>();

    override void OnInit()
    {
        super.OnInit();
        AloneZStagesConfig.Load();
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        m_AloneZTickAcc += timeslice;
        m_AloneZMinuteAcc += timeslice;

        if (m_AloneZTickAcc < 3.0) return;
        m_AloneZTickAcc = 0;

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players[i]);
            if (!pb || !pb.GetIdentity()) continue;
            string uid = pb.GetIdentity().GetPlainId();

            PlayerBase last = null;
            if (m_AloneZSeenPB.Contains(uid)) last = m_AloneZSeenPB.Get(uid);
            if (last != pb)
            {
                m_AloneZSeenPB.Set(uid, pb);
                m_AloneZSynced.Set(uid, false);
            }
            bool did = false;
            if (m_AloneZSynced.Contains(uid)) did = m_AloneZSynced.Get(uid);
            if (!did)
            {
                AloneZPlayerData d0 = AloneZPlayerDB.Load(uid);
                AloneZProgressSrv.SyncAll(pb, uid, d0);
                m_AloneZLastPos.Set(uid, pb.GetPosition());
                m_AloneZDistAcc.Set(uid, 0);
                m_AloneZSynced.Set(uid, true);
            }

            vector cur = pb.GetPosition();
            if (m_AloneZLastPos.Contains(uid))
            {
                vector lastPos = m_AloneZLastPos.Get(uid);
                vector diff = cur - lastPos;
                float dist = diff.Length();
                if (!pb.IsAlive() || dist > 300.0)
                {
                    m_AloneZLastPos.Set(uid, cur);
                    m_AloneZDistAcc.Set(uid, 0);
                }
                else
                {
                    float acc = 0;
                    if (m_AloneZDistAcc.Contains(uid)) acc = m_AloneZDistAcc.Get(uid);
                    acc += dist;
                    int units = Math.Floor(acc / 100.0);
                    if (units > 0)
                    {
                        acc -= units * 100.0;
                        m_AloneZDistAcc.Set(uid, acc);
                        AloneZProgressSrv.AddProgress(pb, uid, AloneZCat.DIST, units);
                    }
                    else
                    {
                        m_AloneZDistAcc.Set(uid, acc);
                    }
                    m_AloneZLastPos.Set(uid, cur);
                }
            }
            else
            {
                m_AloneZLastPos.Set(uid, cur);
                m_AloneZDistAcc.Set(uid, 0);
            }
        }

        while (m_AloneZMinuteAcc >= 60.0)
        {
            m_AloneZMinuteAcc -= 60.0;
            for (int j = 0; j < players.Count(); j++)
            {
                PlayerBase _pb = PlayerBase.Cast(players[j]);
                if (!_pb || !_pb.GetIdentity()) continue;
                string idu = _pb.GetIdentity().GetPlainId();
                AloneZProgressSrv.AddProgress(_pb, idu, AloneZCat.TIME, 1);

                int hmin = 0;
                if (m_AloneZHourMin.Contains(idu)) hmin = m_AloneZHourMin.Get(idu);
                hmin += 1;
                if (hmin >= 60)
                {
                    hmin = 0;
                    AloneZProgressSrv.GiveHourlyRewards(_pb, idu);
                }
                m_AloneZHourMin.Set(idu, hmin);
            }
        }
    }

    override void OnEvent(EventType eventTypeId, Param params)
    {
        super.OnEvent(eventTypeId, params);

        if (eventTypeId == ChatMessageEventTypeID)
        {
            ChatMessageEventParams chatParams = ChatMessageEventParams.Cast(params);
            if (!chatParams) return;
            string senderName = chatParams.param2;
            string text = chatParams.param3;
            if (text.IndexOf("!alonez") != 0) return;

            array<Man> chatPlayers = new array<Man>();
            GetGame().GetPlayers(chatPlayers);

            for (int c = 0; c < chatPlayers.Count(); c++)
            {
                PlayerBase cpb = PlayerBase.Cast(chatPlayers[c]);
                if (!cpb || !cpb.GetIdentity()) continue;
                if (cpb.GetIdentity().GetName() == senderName)
                {
                    string cuid = cpb.GetIdentity().GetPlainId();
                    AloneZProgressSrv.HandleChatCommand(cpb, cuid, text);
                    break;
                }
            }
        }
    }
}
