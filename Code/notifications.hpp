// imgui-notify by patrickcjk
// https://github.com/patrickcjk/imgui-notify

#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

#pragma once
#include <vector>
#include <string>
#include "fonts.hpp"

#define NOTIFY_MAX_MSG_LENGTH			4096		// Max message content length
#define NOTIFY_PADDING_X				20.f		// Bottom-left X padding
#define NOTIFY_PADDING_Y				20.f		// Bottom-left Y padding
#define NOTIFY_PADDING_MESSAGE_Y		10.f		// Padding Y between each message
#define NOTIFY_FADE_IN_OUT_TIME			150			// Fade in and out duration
#define NOTIFY_DEFAULT_DISMISS			3000		// Auto dismiss after X ms (default, applied only of no data provided in constructors)
#define NOTIFY_OPACITY					1.0f		// 0-1 notification opacity
#define NOTIFY_notification_FLAGS				ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing// Comment out if you don't want any separator between title and content
#define NOTIFY_USE_SEPARATOR

#define NOTIFY_INLINE					inline
#define NOTIFY_NULL_OR_EMPTY(str)		(!str ||! strlen(str))
#define NOTIFY_FORMAT(fn, format, ...)	if (format) { va_list args; va_start(args, format); fn(format, args, __VA_ARGS__); va_end(args); }

typedef int ImGuiNotificationType;
typedef int ImGuiNotificationPhase;
enum ImGuiNotificationType_
{
	ImGuiNotificationType_None,
	ImGuiNotificationType_Success,
	ImGuiNotificationType_Warning,
	ImGuiNotificationType_Error,
	ImGuiNotificationType_ASI_Success,
	ImGuiNotificationType_ASI_Error,
	ImGuiNotificationType_ASI_Info,
	ImGuiNotificationType_Info,
	ImGuiNotificationType_COUNT
};

enum ImGuiNotificationPhase_
{
	ImGuiNotificationPhase_FadeIn,
	ImGuiNotificationPhase_Wait,
	ImGuiNotificationPhase_FadeOut,
	ImGuiNotificationPhase_Expired,
	ImGuiNotificationPhase_COUNT
};

class ImGuiNotification
{
private:
	ImGuiNotificationType	type = ImGuiNotificationType_None;
	char			title[NOTIFY_MAX_MSG_LENGTH];
	char			content[NOTIFY_MAX_MSG_LENGTH];
	int				dismiss_time = NOTIFY_DEFAULT_DISMISS;
	uint64_t		creation_time = 0;

private:
	// Setters

	NOTIFY_INLINE auto set_title(const char* format, va_list args) { vsnprintf(this->title, sizeof(this->title), format, args); }

	NOTIFY_INLINE auto set_content(const char* format, va_list args) { vsnprintf(this->content, sizeof(this->content), format, args); }

public:

	NOTIFY_INLINE auto set_title(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_title, format); }

	NOTIFY_INLINE auto set_content(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_content, format); }

	NOTIFY_INLINE auto set_type(const ImGuiNotificationType& type) -> void { IM_ASSERT(type < ImGuiNotificationType_COUNT); this->type = type; };

public:
	// Getters

	NOTIFY_INLINE auto get_title() -> char* { return this->title; };

	NOTIFY_INLINE auto get_default_title() -> char*
	{
		if (!strlen(this->title))
		{
			switch (this->type)
			{
			case ImGuiNotificationType_None:
				return NULL;
			case ImGuiNotificationType_Success:
				return "Success";
			case ImGuiNotificationType_Warning:
				return "Warning";
			case ImGuiNotificationType_Error:
				return "Error";
			case ImGuiNotificationType_Info:
				return "Info";
			case ImGuiNotificationType_ASI_Success:
			case ImGuiNotificationType_ASI_Error:
			case ImGuiNotificationType_ASI_Info:
				return "ASI Engine";
			}
		}

		return this->title;
	};

	NOTIFY_INLINE auto get_type() -> const ImGuiNotificationType& { return this->type; };

	NOTIFY_INLINE auto get_color() -> const ImVec4&
	{
		switch (this->type)
		{
		case ImGuiNotificationType_None:
			return { 255, 255, 255, 255 }; // White
		case ImGuiNotificationType_Success:
			return { 0, 255, 0, 255 }; // Green
		case ImGuiNotificationType_Warning:
			return { 255, 255, 0, 255 }; // Yellow
		case ImGuiNotificationType_Error:
			return { 255, 0, 0, 255 }; // Error
		case ImGuiNotificationType_Info:
			return { 0, 157, 255, 255 }; // Blue
		case ImGuiNotificationType_ASI_Error:
			return { 255, 0, 0, 255 }; // Error
		case ImGuiNotificationType_ASI_Success:
			return { 0, 255, 0, 255 }; // Green
		case ImGuiNotificationType_ASI_Info:
			return { 0, 157, 255, 255 }; // Blue
		default:
			return { 255, 255, 255, 255 }; // White

		}
	}

	NOTIFY_INLINE auto get_icon() -> const char*
	{
		switch (this->type)
		{
		case ImGuiNotificationType_None:
			return NULL;
		case ImGuiNotificationType_Success:
		case ImGuiNotificationType_ASI_Success:
			return ICON_FA_CHECK_CIRCLE;
		case ImGuiNotificationType_Warning:
			return ICON_FA_EXCLAMATION_TRIANGLE;
		case ImGuiNotificationType_Error:
		case ImGuiNotificationType_ASI_Error:
			return ICON_FA_TIMES_CIRCLE;
		case ImGuiNotificationType_Info:
		case ImGuiNotificationType_ASI_Info:
			return ICON_FA_INFO_CIRCLE;
		default:
			return NULL;
		}
	}

	NOTIFY_INLINE auto get_content() -> char* { return this->content; };

	NOTIFY_INLINE auto get_elapsed_time() { return GetTickCount64() - this->creation_time; }

	NOTIFY_INLINE auto get_phase() -> const ImGuiNotificationPhase&
	{
		const auto elapsed = get_elapsed_time();

		if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time + NOTIFY_FADE_IN_OUT_TIME)
		{
			return ImGuiNotificationPhase_Expired;
		}
		else if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time)
		{
			return ImGuiNotificationPhase_FadeOut;
		}
		else if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
		{
			return ImGuiNotificationPhase_Wait;
		}
		else
		{
			return ImGuiNotificationPhase_FadeIn;
		}
	}

	NOTIFY_INLINE auto get_fade_percent() -> const float
	{
		const auto phase = get_phase();
		const auto elapsed = get_elapsed_time();

		if (phase == ImGuiNotificationPhase_FadeIn)
		{
			return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
		}
		else if (phase == ImGuiNotificationPhase_FadeOut)
		{
			return (1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismiss_time) / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY;
		}

		return 1.f * NOTIFY_OPACITY;
	}

public:
	// Constructors

	ImGuiNotification(ImGuiNotificationType type, int dismiss_time = NOTIFY_DEFAULT_DISMISS)
	{
		IM_ASSERT(type < ImGuiNotificationType_COUNT);

		this->type = type;
		this->dismiss_time = dismiss_time;
		this->creation_time = GetTickCount64();

		memset(this->title, 0, sizeof(this->title));
		memset(this->content, 0, sizeof(this->content));
	}

	ImGuiNotification(ImGuiNotificationType type, const char* format, ...) : ImGuiNotification(type) { NOTIFY_FORMAT(this->set_content, format); }

	ImGuiNotification(ImGuiNotificationType type, int dismiss_time, const char* format, ...) : ImGuiNotification(type, dismiss_time) { NOTIFY_FORMAT(this->set_content, format); }
};

namespace ImGui
{
	NOTIFY_INLINE std::vector<ImGuiNotification> notifications;

	/// <summary>
	/// Insert a new notification in the list
	/// </summary>
	NOTIFY_INLINE VOID InsertNotification(const ImGuiNotification& notification)
	{
		notifications.push_back(notification);
	}

	/// <summary>
	/// Remove a notification from the list by its index
	/// </summary>
	/// <param name="index">index of the notification to remove</param>
	NOTIFY_INLINE VOID RemoveNotification(int index)
	{
		notifications.erase(notifications.begin() + index);
	}

	/// <summary>
	/// Render notifications, call at the end of your rendering!
	/// </summary>
	NOTIFY_INLINE VOID RenderNotifications()
	{
		const auto vp_size = GetMainViewport()->Size;

		float height = 0.f;

		for (auto i = 0; i < notifications.size(); i++)
		{
			auto* current_notification = &notifications[i];

			if (current_notification->get_phase() == ImGuiNotificationPhase_Expired)
			{
				RemoveNotification(i);
				continue;
			}

			const auto icon = current_notification->get_icon();
			const auto title = current_notification->get_title();
			const auto content = current_notification->get_content();
			const auto default_title = current_notification->get_default_title();
			const auto opacity = current_notification->get_fade_percent();

			auto text_color = current_notification->get_color();
			text_color.w = opacity;

			char window_name[50];
			sprintf_s(window_name, "##notification%d", i);

			SetNextWindowBgAlpha(opacity);
			SetNextWindowPos(ImVec2(vp_size.x - NOTIFY_PADDING_X, vp_size.y - NOTIFY_PADDING_Y - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
			PushStyleColor(ImGuiCol_Border, { 1.f, 1.f, 1.f, 1.f });
			PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.5f);
			Begin(window_name, NULL, NOTIFY_notification_FLAGS);
			{
				ImGui::GetStyle().WindowPadding = ImVec2(5, 5);
				PushTextWrapPos(vp_size.x / 3.f); 

				bool was_title_rendered = false;

				if (!NOTIFY_NULL_OR_EMPTY(icon))
				{
					TextColored(text_color, icon);
					was_title_rendered = true;
				}

				if (!NOTIFY_NULL_OR_EMPTY(title))
				{
					if (!NOTIFY_NULL_OR_EMPTY(icon))
						SameLine();

					Text(title); 
					was_title_rendered = true;
				}
				else if (!NOTIFY_NULL_OR_EMPTY(default_title))
				{
					if (!NOTIFY_NULL_OR_EMPTY(icon))
						SameLine();

					Text(default_title); 
					was_title_rendered = true;
				}
				if (was_title_rendered && !NOTIFY_NULL_OR_EMPTY(content))
				{
					SetCursorPosY(GetCursorPosY() + 5.f);
				}
				if (!NOTIFY_NULL_OR_EMPTY(content))
				{
					if (was_title_rendered)
					{
#ifdef NOTIFY_USE_SEPARATOR
						PushStyleColor(ImGuiCol_Separator, { 1.f, 1.f, 1.f, 1.f });
						Separator();
						PopStyleColor();

#endif
					}

					Text(content);
				}

				PopTextWrapPos();
			}

			PopStyleColor();
			PopStyleVar();
			height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;

			// End
			End();
		}
	}

}

#endif
