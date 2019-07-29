enum HitOutcome
{

	Start = 0,
	Hit = 1,
	Miss = 2,
	Parry = 3,
	Unresolved = 4,
	Parry_hit = 5,
	Canceled = 6,

}

string hit_outcome_name(int hit_outcome)
{
	if(hit_outcome == HitOutcome::Start) return "Start";
	if(hit_outcome == HitOutcome::Hit) return "Hit";
	if(hit_outcome == HitOutcome::Miss) return "Miss";
	if(hit_outcome == HitOutcome::Parry) return "Parry";
	if(hit_outcome == HitOutcome::Unresolved) return "Unresolved";
	if(hit_outcome == HitOutcome::Parry_hit) return "Parry_hit";
	if(hit_outcome == HitOutcome::Canceled) return "Canceled";

	return "";
}