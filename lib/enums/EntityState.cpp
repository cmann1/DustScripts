enum EntityState
{
	
	Idle				= 0,
	Victory				= 1,
	Run					= 2,
	Skid				= 3,
	Superskid			= 4,
	Fall				= 5,
	Land				= 6,
	Hover				= 7,
	Jump				= 8,
	Dash				= 9,
	CrouchJump			= 10,
	WallRun				= 11,
	WallGrab			= 12,
	WallGrabIdle		= 13,
	WallGrabRelease		= 14,
	RoofGrab			= 15,
	RoofGrabIdle		= 16,
	RoofRun				= 17,
	SlopeSlide			= 18,
	Raise				= 19,
	Stun				= 20,
	StunWall			= 21,
	StunGround			= 22,
	SlopeRun			= 23,
	Hop					= 24,
	Spawn				= 25,
	Fly					= 26,
	ThanksCleansed		= 27,
	IdleCleansed		= 28,
	IdleCleansedThanks	= 29,
	FallCleansed		= 30,
	LandCleansed		= 31,
	Cleansed			= 32,
	Block				= 33,
	WallDash			= 34,
	
}

string entity_state_name(int state)
{
	if(state == EntityState::Idle) return "Idle";
	if(state == EntityState::Victory) return "Victory";
	if(state == EntityState::Run) return "Run";
	if(state == EntityState::Skid) return "Skid";
	if(state == EntityState::Superskid) return "Superskid";
	if(state == EntityState::Fall) return "Fall";
	if(state == EntityState::Land) return "Land";
	if(state == EntityState::Hover) return "Hover";
	if(state == EntityState::Jump) return "Jump";
	if(state == EntityState::Dash) return "Dash";
	if(state == EntityState::CrouchJump) return "CrouchJump";
	if(state == EntityState::WallRun) return "WallRun";
	if(state == EntityState::WallGrab) return "WallGrab";
	if(state == EntityState::WallGrabIdle) return "WallGrabIdle";
	if(state == EntityState::WallGrabRelease) return "WallGrabRelease";
	if(state == EntityState::RoofGrab) return "RoofGrab";
	if(state == EntityState::RoofGrabIdle) return "RoofGrabIdle";
	if(state == EntityState::RoofRun) return "RoofRun";
	if(state == EntityState::SlopeSlide) return "SlopeSlide";
	if(state == EntityState::Raise) return "Raise";
	if(state == EntityState::Stun) return "Stun";
	if(state == EntityState::StunWall) return "StunWall";
	if(state == EntityState::StunGround) return "StunGround";
	if(state == EntityState::SlopeRun) return "SlopeRun";
	if(state == EntityState::Hop) return "Hop";
	if(state == EntityState::Spawn) return "Spawn";
	if(state == EntityState::Fly) return "Fly";
	if(state == EntityState::ThanksCleansed) return "ThanksCleansed";
	if(state == EntityState::IdleCleansed) return "IdleCleansed";
	if(state == EntityState::IdleCleansedThanks) return "IdleCleansedThanks";
	if(state == EntityState::FallCleansed) return "FallCleansed";
	if(state == EntityState::LandCleansed) return "LandCleansed";
	if(state == EntityState::Cleansed) return "Cleansed";
	if(state == EntityState::Block) return "Block";
	if(state == EntityState::WallDash) return "WallDash";
	
	return "";
}