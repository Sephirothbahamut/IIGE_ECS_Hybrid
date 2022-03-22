Attaching system

Game_object

std::vector<Game_object*> owned_game_objects; //list of all objects attached to this objects
~Game_object() { foreach(owned_game_objects) { owned_game_object.detach(); }}
attach(Game_object* new_owner) 
	{
	attached_to;
	if(attached_to = get_component<attached_to>(this.entity))
		{
		attached_to.owner.remove_from_attached_list(this);
		}
	else
		{
		attached_to = add_component<attached_to>(this.entity);
		add_component<world_transform>(this.entity);
		}
	attached_to.owner = new_owner;
	}
detach()
	{
	if(attached_to = get_component<attached_to>(this.entity))
		{
		attached_to.owner.remove_from_attached_list(this);
		remove_component<attached_to>(this.entity);
		remove_component<world_transform>(this.entity);
		}
	}
	
component attached_to { Game_object* owner; }
component world_transform : public transform {};