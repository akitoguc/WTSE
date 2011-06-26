#include "stdafx.h"

#include <algorithm>
//#include <ntsecapi.h>
#include <wtsapi32.h>

#include "LogonSession.h"

using namespace LogonSession;

using std::wstring;
using std::vector;

LPCWSTR InfoEntry::seperator = L"|";
LPCWSTR InfoEntry::terminator = L";";

InfoEntry::InfoEntry(void)
{
    ZeroMemory(&m_logonTime, sizeof(LARGE_INTEGER));
}

InfoEntry::~InfoEntry(void)
{
}

void InfoEntry::SetUserName(LPWSTR wstr)
{
    m_wstrUserName = wstring(wstr);
}

void InfoEntry::SetDomain(LPWSTR wstr)
{
    m_wstrDomain = wstring(wstr);
}

void InfoEntry::SetWinStationName(LPWSTR wstr)
{
    m_wstrWinStationName = wstring(wstr);
}

void InfoEntry::SetConnectionState(UINT state)
{
    m_connectionState = state;
}

void InfoEntry::SetClientName(LPWSTR wstr)
{
    m_wstrClientName = wstring(wstr);
}

void InfoEntry::SetLogonTime(LARGE_INTEGER time)
{
    m_logonTime = time;
}

LPCWSTR InfoEntry::GetUserName() const
{
    return m_wstrUserName.c_str();
}

LPCWSTR InfoEntry::GetDomain() const
{
    return m_wstrDomain.c_str();
}

LPCWSTR InfoEntry::GetWinStationName() const
{
    return m_wstrWinStationName.c_str();
}

LPCWSTR InfoEntry::GetClientName() const
{
    return m_wstrClientName.c_str();
}

LARGE_INTEGER InfoEntry::GetLogonTime() const
{
    return m_logonTime;
}

UINT InfoEntry::GetConnectionState() const
{
    return m_connectionState;
}

std::wstring InfoEntry::ToString() const
{
    wstring wstr;
    wstr += m_wstrUserName;
    wstr += wstring(seperator);
    wstr += m_wstrDomain;
    wstr += wstring(seperator);
    wstr += m_wstrWinStationName;
    wstr += wstring(seperator);
    wstr += m_wstrClientName;
    wstr += wstring(seperator);
    wstr += FormatTime(m_logonTime);
    wstr += wstring(seperator);
    wstr += FormatValue(m_connectionState);
    wstr += wstring(seperator);
    return wstr;
}

std::wstring InfoEntry::FormatTime(LARGE_INTEGER nTime) const
{
    size_t size = sizeof(nTime);

    wstring wstr;
    copy(reinterpret_cast<char*>(&nTime), reinterpret_cast<char*>(&nTime) + size, back_inserter(wstr));
    wchar_t* pwstr = &wstr[0];

    return wstr;
}

bool InfoEntry::ParseString(const wstring& wstr)
{
    wstring* members[] = {
        &m_wstrUserName,
        &m_wstrDomain,
        &m_wstrWinStationName,
        &m_wstrClientName
    };
    int size = sizeof(members) / sizeof(wstring*);

    size_t begin = 0, end = 0;

    for (int i = 0; i < size; ++i) {
        end = wstr.find(seperator, begin);
        if (wstring::npos == end) {
            return false;
        }
        members[i]->assign(wstr, begin, end - begin);
        begin = end + 1;
    }

    // logon time
    end = begin + sizeof(LARGE_INTEGER);
    vector<char> buffer;
    struct CastWchar2Char { char operator()(wchar_t x) const { return static_cast<char>(x); } } cast;
    std::transform(&wstr[begin], &wstr[end], back_inserter(buffer), cast);
    m_logonTime = *(reinterpret_cast<LARGE_INTEGER*>(&buffer[0]));

    // active or not
    begin = end + 1;
    end = wstr.find(seperator, begin);
    if (wstring::npos == end) {
        return false;
    }
    m_connectionState = static_cast<UINT>(_wtoi(wstring(wstr, begin, end - begin).c_str()));

    return true;
}

bool InfoEntry::operator==(const InfoEntry& x) const
{
    /// @note   doesn't compare active status
    return (m_wstrUserName == x.m_wstrUserName
            && m_wstrDomain == x.m_wstrDomain
            && m_wstrWinStationName == x.m_wstrWinStationName
            && m_wstrClientName == x.m_wstrClientName
            // && 0 == CompareFileTime(reinterpret_cast<const FILETIME*>(&m_logonTime), reinterpret_cast<const FILETIME*>(&x.m_logonTime))
            && m_connectionState == x.m_connectionState
    );
}


bool InfoEntry::operator<(const InfoEntry& x) const
{
    // interactive logon type has top priority!
    if (m_connectionState != x.m_connectionState) {
        if (static_cast<WTS_CONNECTSTATE_CLASS>(m_connectionState) == WTSActive) {
            return true;
        }
        if (static_cast<WTS_CONNECTSTATE_CLASS>(x.m_connectionState) == WTSActive) {
            return false;
        }
    }

    if (m_wstrUserName != x.m_wstrUserName) {
        return m_wstrUserName < x.m_wstrUserName;
    }
    else if (m_wstrDomain != x.m_wstrDomain) {
        m_wstrDomain < x.m_wstrDomain;
    }
    else if (m_wstrWinStationName != x.m_wstrWinStationName) {
        return m_wstrWinStationName < x.m_wstrWinStationName;
    }
    else if (m_wstrClientName != x.m_wstrClientName) {
        return m_wstrClientName < x.m_wstrClientName;
    }
//    else if (CompareFileTime(reinterpret_cast<const FILETIME*>(&m_logonTime),
//                             reinterpret_cast<const FILETIME*>(&x.m_logonTime)) == -1) {
//        return true;
//    }
    else if (m_connectionState != x.m_connectionState) {
        return m_connectionState < x.m_connectionState;
    }

    return false;
}

wstring InfoEntry::SerializeToString(const InfoList& list)
{
    wstring wstr;
    InfoList::const_iterator cit = list.begin();
    for (; cit != list.end(); ++cit) {
        wstr += ((*cit).ToString() + terminator);
    }

    return wstr;
}

InfoList InfoEntry::RetrieveFromString(const std::wstring& wstr)
{
    InfoList list;

    size_t begin = 0, end = 0;
    while (true) {
        end = wstr.find(terminator, begin);
        if (wstring::npos == end) {
            break;
        }

        InfoEntry info;
        if (info.ParseString(wstring(wstr, begin, end - begin))) {
            list.push_back(info);
        }
        else {
            fwprintf(stderr, L"Error: parse string: %s \n", wstring(wstr[begin], wstr[end]).c_str());
        }
        begin = end + 1;
    }

    return list;
}

bool InfoEntry::IsInvalid(const InfoEntry& x)
{
    return (0 == wcscmp(x.GetUserName(), L"") && 0 == wcscmp(x.GetDomain(), L"")) ? true : false;
}
