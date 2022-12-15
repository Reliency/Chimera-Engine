#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsdkver.h>
#include <sdkddkver.h>
#include <windows.h>
#include <winternl.h>
#include <d3d11.h>

#include <cassert>
#include <cinttypes>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include "logger.hpp"
#include "notifications.hpp"
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <imgui.h>
#include "toggles.hpp"
#include <wrl/client.h>
template <typename T>
using comptr = Microsoft::WRL::ComPtr<T>;

#define Chimera_WIDE_IMPL(str) L ## str
#define Chimera_WIDE(str) Chimera_WIDE_IMPL(str)

#define Chimera_STRINGIZE_IMPL(x) #x
#define Chimera_STRINGIZE(x) Chimera_STRINGIZE_IMPL(x)

#define Chimera_NAME "Chimera"

namespace Chimera
{
	using namespace std::chrono_literals;
	namespace fs = std::filesystem;

	inline HMODULE g_Module{};
	inline HANDLE g_Thread{};
	inline DWORD g_Thread_ID{};

	inline std::atomic_bool g_Running = true;
	inline HWND g_hwnd{};
	inline std::uint32_t g_SelectedPlayer{};

	inline bool g_LogScriptEvents = false;

}
#include "Logger.hpp"
#include "Util.hpp"
