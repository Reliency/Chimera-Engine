#include "Hooking.hpp"
#include "net_event.hpp"
#include "protections.hpp"
#include "UI/UIManager.hpp"
#include "fwddec.h"
using namespace Chimera;
class datBitBuffer
{
public:
	datBitBuffer(void* data, uint32_t size) {
		m_data = data;
		m_bitOffset = 0;
		m_maxBit = size * 8;
		m_bitsRead = 0;
		m_curBit = 0;
		m_highestBitsRead = 0;
		m_flagBits = 0;
	}
	uint32_t GetPosition() {
		return m_bitsRead;
	}
	bool Seek(uint32_t bits) {
		if (bits >= 0) {
			uint32_t length = (m_flagBits & 1) ? m_maxBit : m_curBit;
			if (bits <= length)
				m_bitsRead = bits;
		}
		return false;
	}
	bool WriteBool(bool integer) {
		return g_Hooking->m_write_bitbuf_bool(this, integer, 1);
	}
	bool ReadBool(bool* integer) {
		return g_Hooking->m_read_bitbuf_bool(this, integer, 1);
	}
	bool ReadPeerId(uint64_t* integer) {
		return this->ReadQWord(integer, 0x40);
	}
	uint64_t ReadBits(size_t numBits) {
		auto const totalBits = (m_flagBits & 1) ? m_maxBit : m_curBit;
		if ((m_flagBits & 2) || m_bitsRead + numBits > totalBits)
			return 0;
		auto const bufPos = m_bitsRead + m_bitOffset;
		auto const initialBitOffset = bufPos & 0b111;
		auto const start = &((uint8_t*)m_data)[bufPos / 8];
		auto const next = &start[1];
		auto result = (start[0] << initialBitOffset) & 0xff;
		for (auto i = 0; i < ((numBits - 1) / 8); i++) {
			result <<= 8;
			result |= next[i] << initialBitOffset;
		}
		if (initialBitOffset)
			result |= next[0] >> (8 - initialBitOffset);
		m_bitsRead += static_cast<uint32_t>(numBits);
		if (m_bitsRead > m_highestBitsRead)
			m_highestBitsRead = m_bitsRead;
		return result >> ((8 - numBits) % 8);
	}
	int GetDataLength() {
		int leftoverBit = (m_curBit % 8) ? 1 : 0;
		return (m_curBit / 8) + leftoverBit;
	}
	bool ReadString(char* string, int bits) {
		return g_Hooking->m_read_bitbuf_string(this, string, bits);
	}
	bool WriteByte(uint8_t integer, int bits) {
		return g_Hooking->m_write_bitbuf_dword(this, integer, bits);
	}
	bool ReadByte(uint8_t* integer, int bits) {
		uint32_t read;
		if (g_Hooking->m_read_bitbuf_dword(this, &read, bits)) {
			*integer = read;
			return true;
		}
		return false;
	}
	bool WriteWord(uint16_t integer, int bits) {
		return g_Hooking->m_write_bitbuf_dword(this, integer, bits);
	}
	bool ReadWord(uint16_t* integer, int bits) {
		uint32_t read;
		if (g_Hooking->m_read_bitbuf_dword(this, &read, bits)) {
			*integer = read;
			return true;
		}
		return false;
	}
	bool WriteDword(uint32_t integer, int bits) {
		return g_Hooking->m_write_bitbuf_dword(this, integer, bits);
	}
	bool ReadDword(uint32_t* integer, int bits) {
		return g_Hooking->m_read_bitbuf_dword(this, integer, bits);
	}
	bool WriteInt32(int32_t integer, int bits) {
		return g_Hooking->m_write_bitbuf_int32(this, integer, bits);
	}
	bool ReadInt32(int32_t* integer, int bits) {
		int32_t v8;
		int32_t v9;
		if (ReadDword((uint32_t*)&v8, 1u) && ReadDword((uint32_t*)&v9, bits - 1)) {
			*integer = v8 + (v9 ^ -v8);
			return true;
		}
		return false;
	}
	bool WriteQWord(uint64_t integer, int bits) {
		return g_Hooking->m_write_bitbuf_qword(this, integer, bits);
	}
	bool ReadQWord(uint64_t* integer, int bits) {
		if (bits <= 32) {
			uint32_t v10;
			if (ReadDword(&v10, bits)) {
				*integer = v10;
				return true;
			}
		}
		else {
			uint32_t v10, v11;
			if (ReadDword(&v11, 32u) && ReadDword(&v10, bits - 32u)) {
				*integer = v11 | ((uint64_t)v10 << 32);
				return true;
			}
		}
		return false;
	}
	bool WriteInt64(int64_t integer, int bits) {
		return g_Hooking->m_write_bitbuf_int64(this, integer, bits);
	}
	bool ReadInt64(int64_t* integer, int bits) {
		uint32_t v8;
		uint64_t v9;
		if (ReadDword(&v8, 1u) && ReadQWord(&v9, bits - 1)) {
			*integer = v8 + (v9 ^ -(int64_t)v8);
			return true;
		}
		return false;
	}
	bool WriteArray(void* array, int size) {
		return g_Hooking->m_write_bitbuf_array(this, array, size, 0);
	}
	bool ReadArray(PVOID array, int size) {
		return g_Hooking->m_read_bitbuf_array(this, array, size, 0);
	}

	template<typename T>
	inline T Read(int length)
	{
		static_assert(sizeof(T) <= 4, "maximum of 32 bit read");

		uint32_t val = 0;
		ReadDword(&val, length);

		return T(val);
	}

	template<typename T>
	inline T ReadSigned(int length)
	{
		static_assert(sizeof(T) <= 4, "maximum of 32 bit read");

		int val = 0;
		ReadInt32(&val, length);

		return T(val);
	}

	template<typename T>
	inline void Write(T data, int length)
	{
		static_assert(sizeof(T) <= 8, "maximum of 64 bit write");

		WriteQWord((uint64_t)data, length);
	}

	template<typename T>
	inline void WriteSigned(int length, T data)
	{
		int sign = data < 0;
		int signEx = (data < 0) ? 0xFFFFFFFF : 0;
		int d = (data ^ signEx);

		Write<int>(1, sign);
		Write<int>(length - 1, d);
	}

	inline float ReadFloat(int length, float divisor)
	{
		auto integer = Read<int>(length);

		float max = (1 << length) - 1;
		return ((float)integer / max) * divisor;
	}

	inline void WriteFloat(int length, float divisor, float value)
	{
		float max = (1 << length) - 1;
		int integer = (int)((value / divisor) * max);

		Write<int>(length, integer);
	}

	inline float ReadSignedFloat(int length, float divisor)
	{
		auto integer = ReadSigned<int>(length);

		float max = (1 << (length - 1)) - 1;
		return ((float)integer / max) * divisor;
	}

	inline void WriteSignedFloat(int length, float divisor, float value)
	{
		float max = (1 << (length - 1)) - 1;
		int integer = (int)((value / divisor) * max);

		WriteSigned<int>(length, integer);
	}
public:
	void* m_data; //0x0000
	uint32_t m_bitOffset; //0x0008
	uint32_t m_maxBit; //0x000C
	uint32_t m_bitsRead; //0x0010
	uint32_t m_curBit; //0x0014
	uint32_t m_highestBitsRead; //0x0018
	uint8_t m_flagBits; //0x001C
};

void scan_explosion_event(CNetGamePlayer* player, datBitBuffer* buffer)
{
	uint16_t f186;
	uint16_t f208;
	int ownerNetId;
	uint16_t f214;
	eExplosionTag explosionType;
	float damageScale;

	float posX;
	float posY;
	float posZ;

	bool f242;
	uint16_t f104;
	float cameraShake;

	bool isAudible;
	bool f189;
	bool isInvisible;
	bool f126;
	bool f241;
	bool f243;

	uint16_t f210;

	float unkX;
	float unkY;
	float unkZ;

	bool f190;
	bool f191;

	uint32_t f164;

	float posX224;
	float posY224;
	float posZ224;

	bool f240;
	uint16_t f218;
	bool f216;

	f186 = buffer->Read<uint16_t>(16);
	f208 = buffer->Read<uint16_t>(13);
	ownerNetId = buffer->Read<uint16_t>(13);
	f214 = buffer->Read<uint16_t>(13); // 1604+
	explosionType = (eExplosionTag)buffer->ReadSigned<int>(8); // 1604+ bit size
	damageScale = buffer->Read<int>(8) / 255.0f;

	posX = buffer->ReadSignedFloat(22, 27648.0f);
	posY = buffer->ReadSignedFloat(22, 27648.0f);
	posZ = buffer->ReadFloat(22, 4416.0f) - 1700.0f;

	f242 = buffer->Read<uint8_t>(1);
	f104 = buffer->Read<uint16_t>(16);
	cameraShake = buffer->Read<int>(8) / 127.0f;

	isAudible = buffer->Read<uint8_t>(1);
	f189 = buffer->Read<uint8_t>(1);
	isInvisible = buffer->Read<uint8_t>(1);
	f126 = buffer->Read<uint8_t>(1);
	f241 = buffer->Read<uint8_t>(1);
	f243 = buffer->Read<uint8_t>(1); // 1604+

	f210 = buffer->Read<uint16_t>(13);

	unkX = buffer->ReadSignedFloat(16, 1.1f);
	unkY = buffer->ReadSignedFloat(16, 1.1f);
	unkZ = buffer->ReadSignedFloat(16, 1.1f);

	f190 = buffer->Read<uint8_t>(1);
	f191 = buffer->Read<uint8_t>(1);

	f164 = buffer->Read<uint32_t>(32);

	if (f242)
	{
		posX224 = buffer->ReadSignedFloat(31, 27648.0f);
		posY224 = buffer->ReadSignedFloat(31, 27648.0f);
		posZ224 = buffer->ReadFloat(31, 4416.0f) - 1700.0f;
	}
	else
	{
		posX224 = 0;
		posY224 = 0;
		posZ224 = 0;
	}

	auto f168 = buffer->Read<uint32_t>(32);		 // >= 1868: f_168


	f240 = buffer->Read<uint8_t>(1);
	if (f240)
	{
		f218 = buffer->Read<uint16_t>(16);

		if (f191)
		{
			f216 = buffer->Read<uint8_t>(8);
		}
	}

	buffer->Seek(0);

	auto object = g_Hooking->m_get_net_object(*g_Hooking->m_network_object_mgr, ownerNetId, true);
	auto entity = object ? object->GetGameObject() : nullptr;

	if (f208 == 0 && entity && reinterpret_cast<CPed*>(entity)->m_player_info && player->m_player_info->m_ped && player->m_player_info->m_ped->m_net_object && ownerNetId != player->m_player_info->m_ped->m_net_object->m_object_id)
	{
		LOG(INFO) << "Received Event: " << "Blamed player";
		g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s blamed %s for an explosion", player->get_name(), reinterpret_cast<CPed*>(entity)->m_player_info->m_net_player_data.m_name });
	}
}

void Hooks::received_event(
	void* event_manager,
	CNetGamePlayer* source_player,
	CNetGamePlayer* target_player,
	uint16_t event_id,
	int event_index,
	int event_handled_bitset,
	int buffer_size,
	datBitBuffer* buffer
)
{
	if (event_id > 91u)
	{
		g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
		return;
	}

	const auto event_name = *(char**)((DWORD64)event_manager + 8i64 * event_id + 243376);
	if (event_name == nullptr || source_player == nullptr || source_player->m_player_id < 0 || source_player->m_player_id >= 32)
	{
		g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
		return;
	}

	switch (static_cast<eNetworkEvents>(event_id))
	{
	case eNetworkEvents::KICK_VOTES_EVENT:
	{
		std::uint32_t player_bitfield = buffer->Read<uint32_t>(32);
		if (player_bitfield & (1 << target_player->m_player_id))
		{
			if (g_Protections.kick_votes_event)
			{
				LOG(INFO) << "Received Event: " << source_player->get_name() << " is voting to kick us.";
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s has voted to kick you", source_player->get_name() });
			}
		}
		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::NETWORK_INCREMENT_STAT_EVENT:
	{
		const auto increment_stat_event = std::make_unique<CNetworkIncrementStatEvent>();
		buffer->ReadDword(&increment_stat_event->m_stat, 0x20);
		buffer->ReadDword(&increment_stat_event->m_amount, 0x20);
		if (Hooks::increment_stat_event(increment_stat_event.get(), source_player))
		{
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}
		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::SCRIPT_ENTITY_STATE_CHANGE_EVENT:
	{
		uint16_t entity = buffer->Read<uint16_t>(13);
		auto type = buffer->Read<ScriptEntityChangeType>(4);
		uint32_t unk = buffer->Read<uint32_t>(32);
		if (type == ScriptEntityChangeType::SettingOfTaskVehicleTempAction)
		{
			uint16_t ped_id = buffer->Read<uint16_t>(13);
			uint32_t action = buffer->Read<uint32_t>(8);

			if ((action >= 15 && action <= 18) || action == 33)
			{
				g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				if (g_Protections.entity_state_crash)
				{
					LOG(INFO) << "Received Event: " << source_player->get_name() << " is voting to kick us.";
					g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s has attempted to crash you.", source_player->get_name() });
				}
				return;
			}
		}
		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::SCRIPTED_GAME_EVENT:
	{
		const auto scripted_game_event = std::make_unique<CScriptedGameEvent>();
		buffer->ReadDword(&scripted_game_event->m_args_size, 32);
		if (scripted_game_event->m_args_size - 1 <= 0x1AF)
			buffer->ReadArray(&scripted_game_event->m_args, 8 * scripted_game_event->m_args_size);

		if (Hooks::scripted_game_event(scripted_game_event.get(), source_player))
		{
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);

			return;
		}
		buffer->Seek(0);

		break;
	}
	case eNetworkEvents::NETWORK_CLEAR_PED_TASKS_EVENT:
	{
		int net_id = buffer->Read<int>(13);
		if (get_local_ped() && get_local_ped()->m_net_object && get_local_ped()->m_net_object->m_object_id == net_id)
		{
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);

			if (g_Protections.freeze_event)
				LOG(INFO) << "Received Event: " << source_player->get_name() << " is voting to freeze us.";
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s has attempted to freeze you.", source_player->get_name() });

			return;
		}

		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::RAGDOLL_REQUEST_EVENT:
	{
		int net_id = buffer->Read<int>(13);

		if (get_local_ped() && get_local_ped()->m_net_object && get_local_ped()->m_net_object->m_object_id == net_id)
		{
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);

			if (g_Protections.ragdoll_event) {
				LOG(INFO) << "Received Event: " << source_player->get_name() << " is voting to ragdoll us.";
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s has attempted to ragdoll you.", source_player->get_name() });
			}
			return;
		}

		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::REPORT_CASH_SPAWN_EVENT:
	{
		uint32_t money;

		buffer->Seek(64);
		buffer->ReadDword(&money, 32);
		buffer->Seek(0);

		if (money >= 2000)
		{
			if (g_Protections.report_cash_event)
			{
				LOG(INFO) << "Received Event: " << source_player->get_name() << " is spawning cash.";
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s is spawning cash.", source_player->get_name() });
			}
		}

		break;
	}
	case eNetworkEvents::REPORT_MYSELF_EVENT:
	{
		if (g_Protections.report_myself_event)
		{
			LOG(INFO) << "Received Event: " << source_player->get_name() << " is modding.";
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s is using a pasted menu.", source_player->get_name() });
		}

		break;
	}
	case eNetworkEvents::REQUEST_CONTROL_EVENT:
	{
		int net_id = buffer->Read<int>(13);
		if (get_local_ped() && get_local_ped()->m_vehicle && get_local_ped()->m_vehicle->m_net_object && get_local_ped()->m_vehicle->m_net_object->m_object_id == net_id)
		{
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			if (g_Protections.request_control_event) {
				LOG(INFO) << "Received Event: " << source_player->get_name() << " tried controlling your vehicle.";
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s tried controlling your vehicle.", source_player->get_name() });
			}
			return;
		}
		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::REMOVE_WEAPON_EVENT:
	{
		int net_id = buffer->Read<int>(13);
		uint32_t hash = buffer->Read<uint32_t>(32);

		if (hash == CONSTEXPR_JOAAT("WEAPON_UNARMED"))
		{
			LOG(INFO) << "Received Event: " << source_player->get_name() << " tried crashing you.";
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s tried crashing you.", source_player->get_name() });
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}

		if (get_local_ped() && get_local_ped()->m_net_object && get_local_ped()->m_net_object->m_object_id == net_id)
		{
			LOG(INFO) << "Received Event: " << source_player->get_name() << " tried removing weapons from you.";
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s tried removing weapons from you.", source_player->get_name() });
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}

		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::REMOVE_ALL_WEAPONS_EVENT:
	{
		int net_id = buffer->Read<int>(13);

		if (get_local_ped() && get_local_ped()->m_net_object && get_local_ped()->m_net_object->m_object_id == net_id)
		{
			LOG(INFO) << "Received Event: " << source_player->get_name() << " tried removing weapons from you.";
			g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s tried removing weapons from you.", source_player->get_name() });
			g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
			return;
		}

		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::GIVE_CONTROL_EVENT:
	{
		uint32_t timestamp = buffer->Read<uint32_t>(32);
		int count = buffer->Read<int>(2);
		bool all_objects_migrate_together = buffer->Read<bool>(1);

		if (count > 3)
		{
			count = 3;
		}

		for (int i = 0; i < count; i++)
		{
			int net_id = buffer->Read<int>(13);
			eNetObjType object_type = buffer->Read<eNetObjType>(4);
			int migration_type = buffer->Read<int>(3);

			if (object_type < eNetObjType::NET_OBJ_TYPE_AUTOMOBILE || object_type > eNetObjType::NET_OBJ_TYPE_TRAIN)
			{
				LOG(INFO) << "Received Event: " << source_player->get_name() << " tried crashing you.";
				g_UiManager->Notification({ ImGuiNotificationType_Warning, 2000, "%s tried crashing you.", source_player->get_name() });
				g_Hooking->m_send_event_ack(event_manager, source_player, target_player, event_index, event_handled_bitset);
				return;
			}
		}

		buffer->Seek(0);
		break;
	}
	case eNetworkEvents::EXPLOSION_EVENT:
	{
		scan_explosion_event(source_player, buffer);
		break;
	}
	default:
		break;
	}
	return static_cast<decltype(&Hooks::received_event)>(g_Hooking->m_OriginalReceivedEvent)(event_manager, source_player, target_player, event_id, event_index, event_handled_bitset, buffer_size, buffer);
}