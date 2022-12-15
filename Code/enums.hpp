#pragma once
enum class eEntityProofs : uint32_t
{
	BULLET = 1 << 4,
	FIRE = 1 << 5,
	COLLISION = 1 << 6,
	MELEE = 1 << 7,
	GOD = 1 << 8,
	EXPLOSION = 1 << 11,
	STEAM = 1 << 15,
	DROWN = 1 << 16,
	WATER = 1 << 24,
};
enum class ePedTask
{
	TASK_NONE,
	TASK_FOOT = 1 << 4,
	TASK_UNK = 1 << 5,
	TASK_DRIVING = 1 << 6
};
enum class eNetworkEvents : uint16_t
{
	OBJECT_ID_FREED_EVENT,
	OBJECT_ID_REQUEST_EVENT,
	ARRAY_DATA_VERIFY_EVENT,
	SCRIPT_ARRAY_DATA_VERIFY_EVENT,
	REQUEST_CONTROL_EVENT,
	GIVE_CONTROL_EVENT,
	WEAPON_DAMAGE_EVENT,
	REQUEST_PICKUP_EVENT,
	REQUEST_MAP_PICKUP_EVENT,
	GAME_CLOCK_EVENT,
	GAME_WEATHER_EVENT,
	RESPAWN_PLAYER_PED_EVENT,
	GIVE_WEAPON_EVENT,
	REMOVE_WEAPON_EVENT,
	REMOVE_ALL_WEAPONS_EVENT,
	VEHICLE_COMPONENT_CONTROL_EVENT,
	FIRE_EVENT,
	EXPLOSION_EVENT,
	START_PROJECTILE_EVENT,
	UPDATE_PROJECTILE_TARGET_EVENT,
	REMOVE_PROJECTILE_ENTITY_EVENT,
	BREAK_PROJECTILE_TARGET_LOCK_EVENT,
	ALTER_WANTED_LEVEL_EVENT,
	CHANGE_RADIO_STATION_EVENT,
	RAGDOLL_REQUEST_EVENT,
	PLAYER_TAUNT_EVENT,
	PLAYER_CARD_STAT_EVENT,
	DOOR_BREAK_EVENT,
	SCRIPTED_GAME_EVENT,
	REMOTE_SCRIPT_INFO_EVENT,
	REMOTE_SCRIPT_LEAVE_EVENT,
	MARK_AS_NO_LONGER_NEEDED_EVENT,
	CONVERT_TO_SCRIPT_ENTITY_EVENT,
	SCRIPT_WORLD_STATE_EVENT,
	CLEAR_AREA_EVENT,
	CLEAR_RECTANGLE_AREA_EVENT,
	NETWORK_REQUEST_SYNCED_SCENE_EVENT,
	NETWORK_START_SYNCED_SCENE_EVENT,
	NETWORK_STOP_SYNCED_SCENE_EVENT,
	NETWORK_UPDATE_SYNCED_SCENE_EVENT,
	INCIDENT_ENTITY_EVENT,
	GIVE_PED_SCRIPTED_TASK_EVENT,
	GIVE_PED_SEQUENCE_TASK_EVENT,
	NETWORK_CLEAR_PED_TASKS_EVENT,
	NETWORK_START_PED_ARREST_EVENT,
	NETWORK_START_PED_UNCUFF_EVENT,
	NETWORK_SOUND_CAR_HORN_EVENT,
	NETWORK_ENTITY_AREA_STATUS_EVENT,
	NETWORK_GARAGE_OCCUPIED_STATUS_EVENT,
	PED_CONVERSATION_LINE_EVENT,
	SCRIPT_ENTITY_STATE_CHANGE_EVENT,
	NETWORK_PLAY_SOUND_EVENT,
	NETWORK_STOP_SOUND_EVENT,
	NETWORK_PLAY_AIRDEFENSE_FIRE_EVENT,
	NETWORK_BANK_REQUEST_EVENT,
	NETWORK_AUDIO_BARK_EVENT,
	REQUEST_DOOR_EVENT,
	NETWORK_TRAIN_REPORT_EVENT,
	NETWORK_TRAIN_REQUEST_EVENT,
	NETWORK_INCREMENT_STAT_EVENT,
	MODIFY_VEHICLE_LOCK_WORD_STATE_DATA,
	MODIFY_PTFX_WORD_STATE_DATA_SCRIPTED_EVOLVE_EVENT,
	REQUEST_PHONE_EXPLOSION_EVENT,
	REQUEST_DETACHMENT_EVENT,
	KICK_VOTES_EVENT,
	GIVE_PICKUP_REWARDS_EVENT,
	NETWORK_CRC_HASH_CHECK_EVENT,
	BLOW_UP_VEHICLE_EVENT,
	NETWORK_SPECIAL_FIRE_EQUIPPED_WEAPON,
	NETWORK_RESPONDED_TO_THREAT_EVENT,
	NETWORK_SHOUT_TARGET_POSITION,
	VOICE_DRIVEN_MOUTH_MOVEMENT_FINISHED_EVENT,
	PICKUP_DESTROYED_EVENT,
	UPDATE_PLAYER_SCARS_EVENT,
	NETWORK_CHECK_EXE_SIZE_EVENT,
	NETWORK_PTFX_EVENT,
	NETWORK_PED_SEEN_DEAD_PED_EVENT,
	REMOVE_STICKY_BOMB_EVENT,
	NETWORK_CHECK_CODE_CRCS_EVENT,
	INFORM_SILENCED_GUNSHOT_EVENT,
	PED_PLAY_PAIN_EVENT,
	CACHE_PLAYER_HEAD_BLEND_DATA_EVENT,
	REMOVE_PED_FROM_PEDGROUP_EVENT,
	REPORT_MYSELF_EVENT,
	REPORT_CASH_SPAWN_EVENT,
	ACTIVATE_VEHICLE_SPECIAL_ABILITY_EVENT,
	BLOCK_WEAPON_SELECTION,
	NETWORK_CHECK_CATALOG_CRC
};

enum class ScriptEntityChangeType
{
	BlockingOfNonTemporaryEvents,
	SettingOfPedRelationshipGroupHash,
	SettingOfDriveTaskCruiseSpeed,
	SettingOfLookAtEntity,
	SettingOfPlaneMinHeightAboveTerrain,
	SetPedRagdollBlockFlag,
	SettingOfTaskVehicleTempAction,
	SetPedFacialIdleAnimOverride,
	SetVehicleLockState,
	SetVehicleExclusiveDriver
};

enum class eRemoteEvent
{
	Bounty = 1370461707, // (137, "FM_TXT_BNTY0", iVar1, PLAYER::GET_PLAYER_NAME(Var2.f_1), "", 5000, Var2.f_6);
	CeoBan = 316066012, // mpply_vipgameplaydisabledtimer
	CeoKick = -1831959078, // BGDISMISSED
	CeoMoney = 245065909, // Goon_Paid_Large
	ClearWantedLevel = 2080651008,
	ForceMission = 1858712297, // ), Var0.f_2, 1))
	ForceMission2 = -1578682814, // TICK_PH_INVA
	GiveCollectible = 697566862, // DLC_SUM20_HIDDEN_COLLECTIBLES xref
	GtaBanner = -795380017, // NETWORK::NETWORK_IS_SCRIPT_ACTIVE("BUSINESS_BATTLES", -1, true, 0) second one
	NetworkBail = 915462795, // NETWORK::NETWORK_BAIL(16, 0, 0); xref func
	PersonalVehicleDestroyed = -964882004, // PLYVEH_INS_DES1
	RemoteOffradar = -162943635, // NETWORK::GET_TIME_DIFFERENCE(NETWORK::GET_NETWORK_TIME(), Var0.f_2)
	SendToCutscene = 392606458, // (bVar3, bVar4, 125f, 1)
	SendToCayoPerico = -910497748, // CELL_HI_INV
	SendToLocation = 1214823473, // &Var222, 11);
	// SHKick = 1037001637,
	SoundSpam = -1891171016, // CELL_APTINVYACHT
	Spectate = -1903870031, // SPEC_TCK1
	Teleport = -168599209, // Mission_Pass_Notify
	TeleportToWarehouse = 434937615, // .f_4 == 50
	TransactionError = -768108950, // NETWORK_RECEIVE_PLAYER_JOBSHARE_CASH
	VehicleKick = -852914485, // PIM_RFMOC
	// Kick = 1674887089,
	// ChangeMCRole = 656530441, // _PLAYSTATS_CHANGE_MC_ROLE
	// DisableRecording = 867047895, // GET_FINAL_RENDERED_CAM_COORD
	MCTeleport = 879177392, // NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(PLAYER::PLAYER_ID()) == (first one)
	StartActivity = 243072129, // (Var0.f_2, -1); first match
	MarkPlayerAsBeast = -2001677186, // GB_BOSSBST xref == PLAYER... global (sadly doesn't actually turn people into the beast)
	KickFromInterior = 1454834612, // ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), PED::GET_PED_MAX_HEALTH(PLAYER::PLAYER_PED_ID()), false)
	InteriorControl = 1268038438, // eventData.f_3 != _INVALID_PLAYER_INDEX_0()

	Crash = -904555865, // SET_NO_LOADING_SCREEN, xref it
	Crash2 = 1775863255,

	TSECommand = 113023613, // CnCTG_IN_BF
	TSECommandRotateCam = 0x69979fea /*joaat("pats_horse_right") in script*/, // != 29) && f

	Notification = 548471420,
	NotificationMoneyBanked = -1032040118, // TICK_TC_BANK
	NotificationMoneyRemoved = -1197151915, // TICK_TC_REMO
	NotificationMoneyStolen = -28878294, // TICK_TC_STOL
};

enum class eActivityType
{
	HeistPrep = 233,
	Gunrunning = 180,
	Sightseer = 142,
	HeadHunter = 166,
	BuySpecialCargo = 167,
	SellSpecialCargo = 168,
	DefendSpecialCargo = 169,
	StealVehicle = 178,
	ExportVehicle = 188,
	Gunrunning2 = 225,
	GunrunningSell = 226,
	GunrunningDefend = 227,
	BikerSell = 190,
	BikerDefend = 191,
	BusinessResupply = 192,
	Survival = 3,
	Darts = 14,
	ArmWresling = 15,
	GangAttack = 6,
	PilotSchool = 122,
	Golf = 11,
	ShootingRange = 13,
	Tennis = 12,
	BaseJump = 8,
	Deathmatch = 1,
	ImpromptuDeathmatch = 5,
	Mission = 0,
	Race = 2,
	ExecutiveDeathmatch = 148,
	MarkedForDeath = 151,
	PiracyPrevention = 152,
	MostWanted = 153,
	AssetRecovery = 157,
	HostileTakeover = 159,
	Point2Point = 162,
	AmphibiousAssault = 216,
	Velocity = 219,
	GunsForHire = 185,
	ByThePound = 182,
	RippingItUp = 194,
	RaceToPoint = 189,
	HitAndRide = 193,
	CriminalMischief = 205,
	WeaponOfChoice = 186,
	FragileGoods = 207,
	Torched = 208,
	Outrider = 209,
	WheelieRider = 210,
	POW = 183,
	ExecutiveSearch = 199,
	StandYourGround = 201,
	AutoBuyout = 163,
	DueDiligence = 160,
	MarketManipulation = 154,
	CourierService = 155,
};

enum class eNetObjType
{
	NET_OBJ_TYPE_AUTOMOBILE,
	NET_OBJ_TYPE_BIKE,
	NET_OBJ_TYPE_BOAT,
	NET_OBJ_TYPE_DOOR,
	NET_OBJ_TYPE_HELI,
	NET_OBJ_TYPE_OBJECT,
	NET_OBJ_TYPE_PED,
	NET_OBJ_TYPE_PICKUP,
	NET_OBJ_TYPE_PICKUP_PLACEMENT,
	NET_OBJ_TYPE_PLANE,
	NET_OBJ_TYPE_SUBMARINE,
	NET_OBJ_TYPE_PLAYER,
	NET_OBJ_TYPE_TRAILER,
	NET_OBJ_TYPE_TRAIN
};

enum class eNetObjectFlags : std::uint16_t
{
	NET_OBJ_FLAGS_FROM_SCRIPT = 1 << 2,
	NET_OBJ_FLAGS_SCRIPTED = 1 << 6,
};

enum class eAckCode : std::uint32_t
{
	ACKCODE_SUCCESS,
	ACKCODE_FAIL,
	ACKCODE_WRONG_OWNER,
	ACKCODE_OUT_OF_SEQUENCE,
	ACKCODE_NO_OBJECT,
	ACKCODE_TOO_MANY_OBJECTS,
	ACKCODE_CANT_APPLY_DATA,
	ACKCODE_MATCH_NOT_STARTED,
	ACKCODE_NONE
};