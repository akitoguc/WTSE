#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include "WTSEUtility.h"

namespace LogonSession {

    class InfoEntry;

    /// @brief  all info about logon sesssions in a PC
    typedef std::vector<InfoEntry> InfoList;

    /// @brief  data transfer object of logon session
    class WTSEUTILITY_API InfoEntry
    {
        // ctor/dtor
    public:
        InfoEntry(void);
        virtual ~InfoEntry(void);

        // attributes
    public:
        void SetUserName(LPWSTR wstr);
        void SetDomain(LPWSTR wstr);
        void SetWinStationName(LPWSTR wstr);
        void SetClientName(LPWSTR wstr);
        void SetLogonTime(LARGE_INTEGER time);
        void SetConnectionState(UINT state);

        LPCWSTR GetUserName() const;
        LPCWSTR GetDomain() const;
        LPCWSTR GetWinStationName() const;
        LPCWSTR GetClientName() const;
        LARGE_INTEGER GetLogonTime() const;
        UINT GetConnectionState() const;

    public:
        static LPCWSTR seperator;
        static LPCWSTR terminator;

        // operations
    public:
        std::wstring ToString() const;
        bool ParseString(const std::wstring& wstr);

    public:
        bool operator==(const InfoEntry& x) const;
        bool operator<(const InfoEntry& x) const;

    public:
        static std::wstring SerializeToString(const InfoList& array);
        static InfoList RetrieveFromString(const std::wstring& wstr);

        static bool IsInvalid(const InfoEntry& x);

        // implementations
    private:
        // @TODO:
        std::wstring FormatTime(LARGE_INTEGER nTime) const;
        template <typename T> std::wstring FormatValue(const T& i) const;

    private:
        std::wstring m_wstrUserName;
        std::wstring m_wstrDomain;
        std::wstring m_wstrWinStationName;
        std::wstring m_wstrClientName;
        LARGE_INTEGER m_logonTime;
        UINT m_connectionState;
    };

    template <typename T>
    std::wstring InfoEntry::FormatValue(const T& i) const
    {
        std::wostringstream s;
        s << i;
        return s.str();
    }
}
