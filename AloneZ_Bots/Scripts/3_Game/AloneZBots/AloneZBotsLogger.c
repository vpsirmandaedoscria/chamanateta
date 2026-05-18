// ============================================================================
// AloneZ Bots — Sistema de Logs
// Salva logs em $profile:AloneZ/Bots/Logs/
// ============================================================================

class AloneZBotsLogger
{
    static const string LOG_DIR = "$profile:AloneZ/Bots/Logs/";
    static bool m_Initialized = false;
    static bool m_DebugEnabled = false;

    static void Init()
    {
        if (m_Initialized)
            return;

        if (!FileExist(LOG_DIR))
        {
            MakeDirectory("$profile:AloneZ");
            MakeDirectory("$profile:AloneZ/Bots");
            MakeDirectory("$profile:AloneZ/Bots/Logs");
        }

        m_Initialized = true;
    }

    static void SetDebugMode(bool enabled)
    {
        m_DebugEnabled = enabled;
    }

    static void Log(string level, string eventTag, string message)
    {
        Init();

        string timestamp = GetTimestamp();
        string logLine = "[" + timestamp + "] [" + level + "] [" + eventTag + "] " + message;

        if (level == "DEBUG" && !m_DebugEnabled)
            return;

        WriteToDaily(logLine);
        Print("[AloneZ] " + logLine);

        if (level == "WARNING" || level == "ERROR")
        {
            WriteToEvents(logLine);
        }
    }

    static void LogInfo(string eventTag, string message)
    {
        Log("INFO", eventTag, message);
    }

    static void LogWarning(string eventTag, string message)
    {
        Log("WARNING", eventTag, message);
    }

    static void LogError(string eventTag, string message)
    {
        Log("ERROR", eventTag, message);
    }

    static void LogDebug(string eventTag, string message)
    {
        Log("DEBUG", eventTag, message);
    }

    protected static void WriteToDaily(string logLine)
    {
        string date = GetDateString();
        string filePath = LOG_DIR + "AloneZ_Bots_" + date + ".log";
        FileHandle file = OpenFile(filePath, FileMode.APPEND);
        if (file != 0)
        {
            FPrintln(file, logLine);
            CloseFile(file);
        }
    }

    protected static void WriteToEvents(string logLine)
    {
        string eventsPath = LOG_DIR + "AloneZ_Bots_Events.log";
        FileHandle eventsFile = OpenFile(eventsPath, FileMode.APPEND);
        if (eventsFile != 0)
        {
            FPrintln(eventsFile, logLine);
            CloseFile(eventsFile);
        }
    }

    static string GetTimestamp()
    {
        int year, month, day, hour, minute, second;
        GetYearMonthDay(year, month, day);
        GetHourMinuteSecond(hour, minute, second);

        string sMonth = month.ToString();
        string sDay = day.ToString();
        string sHour = hour.ToString();
        string sMinute = minute.ToString();
        string sSecond = second.ToString();

        if (month < 10) sMonth = "0" + sMonth;
        if (day < 10) sDay = "0" + sDay;
        if (hour < 10) sHour = "0" + sHour;
        if (minute < 10) sMinute = "0" + sMinute;
        if (second < 10) sSecond = "0" + sSecond;

        return year.ToString() + "-" + sMonth + "-" + sDay + " " + sHour + ":" + sMinute + ":" + sSecond;
    }

    static string GetDateString()
    {
        int year, month, day;
        GetYearMonthDay(year, month, day);

        string sMonth = month.ToString();
        string sDay = day.ToString();

        if (month < 10) sMonth = "0" + sMonth;
        if (day < 10) sDay = "0" + sDay;

        return year.ToString() + "-" + sMonth + "-" + sDay;
    }
}
