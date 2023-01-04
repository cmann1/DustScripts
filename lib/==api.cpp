/* API methods included here are globally accessible within a script. */
/* Write a string to the console on its own line. */
void puts(const string &in msg);

/* Write a string to the console on its own line with an additional
 * timestamp heading. */
void tputs(const string &in msg);

/* Returns the name of this script. Script names are used as a way of
 * identifiying scripts for use when working with script triggers/enemies. */
string script_name();

/* Returns false in the editor and true in game */
bool is_playing();

/* Returns true if a replay is being watched */
bool is_replay();

/* Use after changing persistent variables via script to update values in the script panel. */
void editor_sync_vars_menu();

/* Return the current scene object.  This scene object will be valid for
 * the entire execution of the script. */
scene@ get_scene();

/* Return the global script object associated with this script. For technical
 * reasons the script must implement the "script_base" interface, otherwise
 * this api will return null. */
script_base@ get_script();

/* Return the camera following player 'player'. Like the scene object this
 * object never needs to be reloaded.
 *
 * Arguments:
 *   :player: The (0-indexed) player index to get the camera of.
 */
camera@ get_camera(uint player);

/* Returns the number of cameras/players currently active. */
uint num_cameras();

/* Get the camera that is currently being viewed. */
camera@ get_active_camera();

/* Get the player index that is currently being viewed. Shorthand for
 * get_active_camera.player(). */
int get_active_player();

/* This is deprecated, use controller_controllable which returns the same
 * result except cast as a controllable. */
entity@ controller_entity(uint player);

/* Return the entity being controlled by player 'player'. This object
 * is no longer valid and should be requeried when a checkpoint is loaded.
 */
controllable@ controller_controllable(uint player);

/* Change the controllable controlled by player 'player'. */
void controller_entity(uint player, controllable@ pl);

/* Reset all camera state based on the player's current position. */
void reset_camera(uint player);

/* Get the player name for the player id. Defaults to "Player " + (player + 1)
 * if not in networking mode or the player name is not known. Returns an empty
 * string if the player does not exist. */
string player_name(int player);

/* Return the entity with the given id. The returned entity object is no
 * longer valid if the entity is removed from the scene either by being
 * destroyed, unloaded, or a checkpoint is loaded. The safest thing to do
 * is requery the entity every frame it will be used.
 *
 * This function will return null if the entity has been destroyed or is not
 * currently loaded. */
entity@ entity_by_id(uint id);

/* Convenience method for entity_by_id that tries to return a controllable. */
controllable@ controllable_by_id(uint id);

/* Convenience method for entity_by_id that tries to return a dustman. */
dustman@ dustman_by_id(uint id);

/* Convenience method for entity_by_id that tries to return a hitbox. */
hitbox@ hitbox_by_id(uint id);

/* Convenience method for entity_by_id that tries to return a script
 * trigger. */
scripttrigger@ scripttrigger_by_id(uint id);

/* Convenience method for entity_by_id that tries to return a script
 * enemy. */
scriptenemy@ scriptenemy_by_id(uint id);

prop@ prop_by_id(uint id);

/* Return the 'index'th entity that will be steped this frame. Should only
 * be called from 'step' and 'step_post'. See * entity_by_id() for notes on
 * liveness of this object. */
entity@ entity_by_index(uint index);

/* Seed the random generator. */
void srand(uint32 sd);

/* Generate a random 30-bit number. */
uint32 rand();

/* Create an tileinfo structure. Defaults to a square virtual tile. */
tileinfo@ create_tileinfo();

/* Create a tilefilth structure. Defaults to no filth on any edge. */
tilefilth@ create_tilefilth();

/* Create a sprites object that can be used to draw sprites to the screen. */
sprites@ create_sprites();

/* Create a prop. */
prop@ create_prop();

/* Create an entity object of the given type. See :doc:`creatable_entities`
 * for a list of entity types that can be created. */
entity@ create_entity(string type_name);

/* Create a new textfield that can be used to draw text to the screen. */
textfield@ create_textfield();

/* Create a new filth ball object. */
filth_ball@ create_filth_ball(int filth_type, float x, float y, float baseWidth, float baseHeight, int dir, float distance);

/* Create a new script trigger backed by the passed trigger_base object. */
scripttrigger@ create_scripttrigger(trigger_base@ obj);

/* Create a new script enemy backed by the passed enemy_base object. */
scriptenemy@ create_scriptenemy(enemy_base@ obj);

/* Create an empty message object. */
message@ create_message();

/* Create a hitbox object. Note that the entity is not automatically added
 * to the scene. However, hitboxes do automatically remove themselves from the
 * scene sometime after activated. Hitboxes cannot be persisted and should be
 * added to the scene with persist set to false. */
hitbox@ create_hitbox(controllable@ owner, float activate_time, float x, float y, float top, float bottom, float left, float right);

canvas@ create_canvas(bool is_hud, int layer, int sub_layer);

/* Add a callback to receive all broadcasted messages with the given id. If id
 * is blank then this receiver will instead receive all messages. */
void add_broadcast_receiver(string id, callback_base@ obj, string methName);

/* Send a message to all registered broadcast receivers. */
void broadcast_message(string id, message@ msg);

/* Returns true if there is an embedded file associated with the passed key.
 *
 * To embed a file into a script use a declaration like
 *   const string EMBED_key = "file.dat"
 *
 * That will seach for the file "file.dat" in embed_src/ and then script_src/.
 * The file data will then be available to be queried by this function,
 * get_embed_value, and can be used as sprite data in build_sprites.
 */
bool has_embed_value(string key);

/* Returns the embedded file data associated with the passed key. See
 * has_embed_value for more details on how to embed a value in a script. */
string get_embed_value(string key);

/* Add/replace the embed key with the file present at
 * "content/plugins/embeds/" + path. Use forward slashes to represent
 * path separation.
 *
 * Returns true if the path was legal and an embed was
 * successfully loaded. If it returns false any existing embed with the same
 * key is unmodified.
 */
bool load_embed(string key, string path);

/* Return the current unix timestamp. */
int timestamp_now();

/* Get the current microseconds. */
uint32 get_time_us();

/* Convert a timestamp to a timedate structure in the local timezone. */
timedate@ localtime(int timestamp);

/* Convert the current time to a timedate structure in the local timezone. */
timedate@ localtime();

/* Convert a timestamp to a timedate structure in the UTC timezone. */
timedate@ gmtime(int timestamp);

/* Convert the current time to a timedate structure in the UTC timezone. */
timedate@ gmtime();

/* Get editor api object if currently in editor mode. */
editor_api@ get_editor_api();

/* Get input api object if currently in editor mode. */
input_api@ get_input_api();

/* Returns a qnexus_api` object if called from within a nexus.
 * Otherwise returns null.
 */
nexus_api@ get_nexus_api();


/* The entry point - each script must have a single class named *script*. */
class script {
  /* Initialize any state variables here. */
  script();

  /* Called just prior to a checkpoint being saved. */
  void checkpoint_save();

  /* Called after a checkpoint has been loaded. All entities and prop objects
   * will have been recreated and therefore existing handles will no longer
   * refer to objects in the scene and should be requeried. */
  void checkpoint_load();

  /* Called when a player hits a checkpoint during multiplayer. */
  void checkpoint_save(int player_index);

  /* Called when a player is respawned during multiplayer. */
  void checkpoint_load(int player_index);

  /* Called when an entity is added to the scene. */
  void entity_on_add(entity@ e);

  /* Called when an entity is removed from the scene. */
  void entity_on_remove(entity@ e);

  /* Called before the entity list to process has been constructed as an
   * opportunity to move the camera. Moving the camera in
   * `script.step` will be too late to affect what segments
   * and entities are loaded and stepped.
   */
  void move_cameras();

  /* Called every game frame (usually 60fps) prior to all entities having their step
   * function called. The list of entities going to be stepped can be accessed
   * with `entity_by_index(i)` for `0 <= i < entities`.
   */
  void step(int entities);

  /* Like step except called after all entities have had their step functions
   * called. */
  void step_post(int entities);

  /* This function is called at 60fps even when the game menu is opened,
   * slowed during fade out, loading checkpoints, etc. This is still
   * subject to manipulation from pausing/changing game speed with frame
   * advance, however. This mirrors the behavior of the menu subsystem.
   */
  void step_fixed();

  /* Setup the camera/transform prior to anything being drawn. Do not actually
   * draw anything here. */
  void pre_draw(float sub_frame);

  /* Do any drawing required by your script. This function should have no side
   * effects outside of the draw calls it makes. */
  void draw(float sub_frame);

  /* Called each frame while in the editor instead of step/step_post. */
  void editor_step();

  /* Called each pre draw frame while in the editor instead of pre_draw. */
  void editor_pre_draw(float sub_frame);

  /* Called each draw frame while in the editor instead of draw. */
  void editor_draw(float sub_frame);

  /* Called when one of this script's variables is modified in the editor */
  void editor_var_changed(var_info@ info);

  /* Spawn a player controllable. The following parameters will be set
   * in the passed message:
   * 
   * :float x: The x coordinate to spawn the player
   * :float y: The y coordinate to spawn the player
   * :int player: The player index of the player
   * :string character: The character id of the selected player
   *   (e.g. "dustgirl")
   * 
   * The following fields can be set to create your player object:
   * 
   * :entity@ player: Set the spawned player controllable.
   * 
   */
  void spawn_player(message@ msg);

  /* Used to set custom sprite data from embedded values. See
   * `has_embed_value()` for more details on how
   * to embed a value. Sprites should be in PNG format.
   *
   * For each sprite you wish to create add a string to the message with the
   * desired sprite name as the key and the embed key as the value.
   * Additionally you can specify custom "offset" coordinates into the sprite
   * to define where the center of the sprite is (defaults to (0, 0)) by
   * setting an int key of the form "sprite_name|offsetx" and
   * "sprite_name|offsety".
   *
   * See https://gist.github.com/msg555/3aaa96428d964c1612b540c208c3ad1e for
   * a complete example on how to embed, build, and use custom sprites.
   */
  void build_sprites(message@ msg);

  /* Userd to set custom sound data from embedded values similar to
   * `build_sprites()`.
   *
   * For each sound you wish to create add a string to the message with the
   * desired sound name as the key and the embed key as the value.
   * Additionally you can specify a custom loop point (measured in samples at
   * 44.1 kHz) by setting an int key of the form "sound_name|loop".
   *
   * See https://gist.github.com/msg555/821c3aec14852e67fd15c7ec96a851f2 for
   * a complete example of how to embed, build, and use custom sounds.
   */
  void build_sounds(message@ msg);

  /* Called when the level begins play either by loading the level normally or
   * by tabbing in from the editor. */
  void on_level_start();

  /* Called when an end condition for the level has been triggered and the
   * replay is about to be uploaded. You may still modify plugin_score at
   * this point. */
  void on_level_end();

  /* Called when entering editor mode. Only applicable to editor plugins */
  void editor_loaded();

  /* Called before entering play mode. Only applicable to editor plugins */
  void editor_unloaded();

}

/* Extend this class to create a new type of script-backed trigger.
 * 
 * Non handle variables can be persisted and made editable in the editor
 * using annotations. These annotated values will be modifiable in the editor
 * and persisted across checkpoints. These values will be set by the time
 * `init()` is called.
 * You should still set sensible defaults in the object's constructor.
 *
 * You can control how a variable is modified in the editor using annotations.
 * Annotations appear before the variable declaration and can take additional
 * parameters afterwards.
 * Variable name labels will be CamelCased and underscores removed by default.
 * Keys or values that have spaces in them must be wrapped in single or double
 * quotes.
 *
 * Multiple annotations can be added to a single variable by separating
 * them with "|": `[attribute1|attribute2|...]`

 * Useful for example when adding a tooltip to a variable that already has
 * other annotations: `[color,alpha|tooltip:'Select a colour']`

 * Certain attributes take a colour value in the form of a hex value,
 * `AARRGGBB` or `ARGB`.
 * Alpha values are optional and will default to 0xFF. The optional prefixes
 * "#"" and "0x" are also allowed.
 *
 * Here is the full list of supported annotations
 * (fields in all caps are meant to be customized).
 *
 * [hidden]
 *   Persist the variable but don't show it in the editor.
 *   Can also be added to classes to hide it in the editor's
 *   type select menu.
 * [label:**TEXT**]
 *   Use **TEXT** for the variables label instead of the variable's name.
 * [tooltip:**TEXT**,delay:DELAY,font:STRING,size:INT,colour/color:COLOUR]
 *   Will display the tooltip **TEXT** after **DELAY** frames (default 20).
 *   Fields with a tooltip will be highlighted in blue.
 *   Optionaly the font, size, and colour can be set. For a list of
 *   valid font/size pairs, see https://pastebin.com/YcNKSXd9
 * [text]
 *   Use a simple text field to modify the variable. This is the default
 *   annotation
 * [option,VALUE1:OPTION1,VALUE2:OPTION2,...]
 *   Use a dropdown option menu. If the user selects **OPTIONk** the
 *   variable's value will be set to **VALUEk**.
 * [angle,MODE]
 *   Use to set an angle. **MODE** can be set to 'rad' or 'radian' to use
 *   radians otherwise it defaults to degrees.
 * [color,alpha] or [colour,alpha]
 *   Use to specify a colour parameter. The optional **alpha** parameter will
 *   display a slider allowing the colour's alpha channel to be changed,
 *   otherwise it will always set to 0xFF.
 * [slider,min:MINVAL,max:MAXVAL,step:STEP]
 *   Use a slider element to set a value between **MINVAL** and **MAXVAL**
 *   uniformly distributed, or with a custom interval using **STEP**.
 * [position,mode:MODE,layer:LAYER,y:YPARAM]
 *   Use this annotation on an x-variable, naming the corresponding y
 *   **variable** as **YPARAM**. **MODE** can be "world" or "hud", defaulting
 *   to "world". **LAYER** is the layer to calculate the coordinates of from
 *   the user's mouse.
 * [entity,TYPE,TYPE,...]
 *   | Use to select an entity id with the mouse.
 *   | An optional list of allowed types can be given. If no **TYPE** type is
 *     specified the defaults are hittable entities and triggers.
 *   | The following values are supported:
 *   |   `default`, `player`, `enemy`, `trigger`, `camera`,
 *       `emitter`, `flags`, `kill_zone`
 * [fixed:MODE]
 *   If present it will not be possible to add or remove items from arrays.
 *   **MODE** can be "all" (default), or "top".
 *   If **MODE** is "top", only the top level of a multidimensional array will
 *   be fixed and subsequent levels will be modifiable.
 *
 * Additionally, bools, arrays, and non-handle classes have the following
 * semantics.
 *
 * bools
 *   Always use a checkbox UI if a non-hidden annotation is supplied.
 *
 * arrays
 *   The annotation applied to the array is instead applied to the value
 *   within the array and an array wrapper UI is used.
 *
 * classes
 *   Always use a class UI if a non-hidden annotation is supplied.
 *
 * Variables will appear in the editor in declaration order. You can
 * override this behavior using the order using an annotation before
 * the start of the class definition that looks like:
 * `[order:[pizza,colour,other_entity_id]]`
 * Any missing parameters will be added afterwards in alphabetical order.
 * 
 * Examples:
 * 
 * .. code-block:: c++
 * 
 *   [entity] int other_entity_id;
 *   [hidden] string my_hidden_string;
 *   [option,0:Cheese,1:Pepperoni,2:Mushroom] int pizza;
 *   [angle] float direction_in_degrees;
 *   [angle,radian] float direction_in_radians;
 *   [colour] int colour;
 *   [slider,min:0,max:55.5] float slider_val;
 *   
 *   [position,mode:world,layer:19,y:pos_y] float pos_x;
 *   [hidden] float pos_y; // Declare the var hidden so it is persisted.
 *
 * Your trigger must have an empty constructor. (unless
 * there are no constructors at all in which a default one is implied) for
 * the trigger to be usable.
 */
class trigger_base {
  /* Called only in the editor when a type is selected, before before init.
   * Can be used to initialise persistent variables which require access to the
   * script or script trigger instance. */
  void editor_init(script@ s, scripttrigger@ self);
  
  /* Called after the trigger is constructed, passing the corresponding game
   * `scripttrigger` handle. */
  void init(script@ s, scripttrigger@ self);
  
  /* Called after the entity has been added to the scene. */
  void on_add();
  
  /* Called after the entity has been removed from the scene. */
  void on_remove();
  
  /* Called when the trigger is stepped. */
  void step();
  
  /* Called when the trigger is stepped while in editor mode. */
  void editor_step();
  
  /* Called when one of this trigger's variables is modified in the editor */
  void editor_var_changed(var_info@ info);
  
  /* Do drawing related to the script trigger. */
  void draw(float sub_frame);
  
  /* Do drawing in the editor related to the script trigger. The base
   * implementation will draw a square for the trigger and, if the activate()
   * function is present, the trigger radius. */
  void editor_draw(float sub_frame);
  
  /* Called when one of this trigger's variables is modified in the editor */
  void editor_var_changed(var_info@ info);
  
  /* Called when any controllable object within the region associated with
   * the trigger. `activate()` is called
   * for each object each frame it is within the trigger. */
  void activate(controllable@ e);
  
  /* Called when a message has been sent to the entity with
   * `entity.send_message(id, @msg)`. */
  void on_message(string id, message@ msg);
}

/* Extend this class to create a new type of script-backed enemy.
 * 
 * See `trigger_base` documentation for discussion on
 * member variables. */
class enemy_base {
  /* Setup initial variables. An empty constructor must be present (unless
   * there are no constructors at all in which a default one is implied) for
   * enemies to be usable. */
  enemy_base();

  /* Called only in the editor when a type is selected, before before init.
   * Can be used to initialise persistent variables which require access to the
   * script or script enemy instance. */
  void editor_init(script@ s, scriptenemy@ self);
  
  /* Called after the enemy is constructed, passing the corresponding game
   * `scriptenemy` handle. */
  void init(script@ s, scriptenemy@ self);

  /* Called after the entity has been added to the scene. */
  void on_add();

  /* Called after the entity has been removed from the scene. */
  void on_remove();

  /* Called when the scale of the object has changed and collisions should be
   * updated. */
  void on_change_scale(float new_scale);

  /* Called when the enemy is stepped. */
  void step();

  /* Called when the enemy is stepped while in editor mode. */
  void editor_step();

  /* Do drawing related to the enemy. */
  void draw(float sub_frame);

  /* Do drawing in the editor related to the enemy trigger. */
  void editor_draw(float sub_frame);

  /* Called when one of this enemy's variables is modified in the editor */
  void editor_var_changed(var_info@ info);

  /* Called when a message has been sent to the entity with
   * `entity.send_message(id, @msg)`. */
  void on_message(string id, message@ msg);

}

class scene {
  /* Get the current level name. */
  string map_name();

  /* Get the current level type. */
  int level_type();

  /* Access the global flag to disable jump. */
  bool jump_enabled();

  void jump_enabled(bool enabled);

  /* Access the global flag to disable attack. */
  bool attack_enabled();

  void attack_enabled(bool enabled);

  /* Access the global flag to disable special. */
  bool special_enabled();

  void special_enabled(bool enabled);

  /* Access the global flag to disable dash. */
  bool dash_enabled();

  void dash_enabled(bool enabled);

  /* Get the current time in level counter (measured in milliseconds). This
   * is the time that is displayed to the user and has those same semantics (e.g.
   * it does not run during startup). You can multiply this by 60/1000 and round
   * to the nearest int to get the approximate number of frames elapsed but note
   * for particuarly long replays accrued floating point errors may make this
   * method eventually give innaccurate results.
   */
  float time_in_level();

  /* Trigger a checkpoint to be saved. Note that the checkpoint is only saved at
   * the start of the next frame.
   * If use_position is false (the default due to a bug and for backwards
   * compatibility reasons) x and y are ignored and the player's current
   * position is used instead. */
  void save_checkpoint(int x, int y, bool use_position);

  /* Trigger a checkpoint to be saved. Note that the checkpoint is only saved at
   * the start of the next frame. */
  void save_checkpoint(int x, int y);

  /* Trigger the last checkpoint to be loaded. If no checkpoint has been set
   * yet the level will be reloaded. */
  void load_checkpoint();

  /* Get the x coordinate for the identified player of where they should respawn
   * on death. */
  float get_checkpoint_x(int player);

  /* Get the y coordinate for the identified player of where they should respawn
   * on death. */
  float get_checkpoint_y(int player);

  /* Get the tileinfo structure for the tile at the given position on
   * layer 19. */
  tileinfo@ get_tile(int x, int y);

  /* Get the tileinfo structure for the tile at the given position and layer. */
  tileinfo@ get_tile(int x, int y, int layer);

  /* Overwrite a tile in the scene.  See tileinfo documentation for what each
   * of these parameters mean.
   */
  void set_tile(int x, int y, int layer, bool solid, int16 type, int16 spriteSet, int16 spriteTile, int16 palette);

  /* Overwrite a tile in the scene using the passed tileinfo structure. */
  void set_tile(int x, int y, int layer, tileinfo@ tile, bool updateEdges);

  /* Get the tilefilth structure for the tile at the given location.
   * Filth includes all things that can be on a side of a tile,
   * i.e. all dust types and all spike types. */
  tilefilth@ get_tile_filth(int x, int y);

  /* Set the filth for a given tile position. See tilefilth documentation for
   * a description of how to interpret the top/bottom/left/right fields. */
  uint set_tile_filth(int x, int y, uint8 top, uint8 bottom, uint8 left, uint8 right, bool affectSpikes, bool overwrite);

  /* Set the filth for a given tile position using a tilefilth object. */
  uint set_tile_filth(int x, int y, tilefilth@ filth);

  /* Project filth onto surfaces using the same line of sight system that is
   * used e.g. to clear dust with attacks. Roughly speaking, the projection will
   * be applied to any tile edge with a center that's within 'distance' pixels
   * from the rectangle centered at (x, y) with size (baseWidth, baseHeight) in
   * the direction of 'direction' +/- spreadAngle.
   *
   * The top/bottom/left/right flags indicate which types of surfaces can be
   * affected.  'affectSpikes' indicates if spikes should be overwritten,
   * 'overwrite' indicates if only edges with no filth should be affected.
   *
   * Affected tiles with have their edge type set to 'type'.  See tilefilth
   * documentation for a description on how to interpret this value. */
  uint project_tile_filth(float x, float y, float baseWidth, float baseHeight, uint8 type, float direction, float distance, float spreadAngle, bool top, bool bottom, bool left, bool right, bool affectSpikes, bool overwrite);

  /* Returns the current default collision layer. Normally this will be layer
   * 19 unless modified. */
  int default_collision_layer();

  /* Sets the default collision layer. Note that this value is not persisted
   * across checkpoints. It is up to the script to set the value appropriately
   * after a checkpoint has been loaded.
   */
  void default_collision_layer(int layer);

  /* Return information about the first tile surface hit from the ray starting
   * at (x1, y1) going to (x2, y2). */
  raycast@ ray_cast_tiles(float x1, float y1, float x2, float y2);

  /* Like the other ray_cast_tiles call except reuse the raycast object result.
   * This avoids unnecessary allocations if you're making a lot of calls per
   * frame. */
  raycast@ ray_cast_tiles(float x1, float y1, float x2, float y2, raycast@ result);

  /* Like the other ray_cast_tiles except provide a layer. */
  raycast@ raycast_ray_cast_tiles_ex(float x1, float y1, float x2, float y2, uint layer);

  /* Like the other ray_cast_tiles except provide a layer. */
  raycast@ ray_cast_tiles_ex(float x1, float y1, float x2, float y2, uint layer, raycast@ result);

  /* Return information about the first tile surface hit from the ray starting
   * at (x1, y1) going to (x2, y2). 'edges' is a bitset indicating which types
   * of edges it should look for collisions with. The 1, 2, 4, and 8 bits
   * correspond to the top, bottom, left, and right edges respectively.
   *
   * Note that the game loads in data for about a 528x528 tile square centered
   * around the camera (multiplayer uses a 144x144 square). Querying anything
   * outside of this loaded region will give no results. */
  raycast@ ray_cast_tiles(float x1, float y1, float x2, float y2, int edges);

  /* Like the other ray_cast_tiles call except reuse the raycast object result.
   * This avoids unnecessary allocations if you're making a lot of calls per
   * frame. */
  raycast@ ray_cast_tiles(float x1, float y1, float x2, float y2, int edges, raycast@ result);

  /* Like the other ray_cast_tiles except provide a layer. */
  raycast@ ray_cast_tiles_ex(float x1, float y1, float x2, float y2, int edges, uint layer);

  /* Like the other ray_cast_tiles except provide a layer. */
  raycast@ ray_cast_tiles_ex(float x1, float y1, float x2, float y2, int edges, uint layer, raycast@ result);

  /* Find the collision of the horizontal line segment (x1, y1), (x2, y1) with
   * tiles as it moves downward to y2. Only collides with ground edges. */
  tilecollision@ collision_ground(float x1, float y1, float x2, float y2);

  /* Find the collision of the horizontal line segment (x1, y1), (x2, y1) with
   * tiles as it moves upward to y2. Only collides with roof edges. */
  tilecollision@ collision_roof(float x1, float y1, float x2, float y2);

  /* Find the collision of the vertical line segment (x1, y1), (x1, y2) with
   * tiles as it moves leftward to x2. Only collides with left edges. */
  tilecollision@ collision_left(float x1, float y1, float x2, float y2);

  /* Find the collision of the vertical line segment (x1, y1), (x1, y2) with
   * tiles as it moves rightward to x2. Only collides with right edges. */
  tilecollision@ collision_right(float x1, float y1, float x2, float y2);

  /* Like collision_ground except provide a layer. */
  tilecollision@ collision_ground_ex(float x1, float y1, float x2, float y2, uint layer);

  /* Like collision_roof except provide a layer. */
  tilecollision@ collision_roof_ex(float x1, float y1, float x2, float y2, uint layer);

  /* Like collision_left except provide a layer. */
  tilecollision@ collision_left_ex(float x1, float y1, float x2, float y2, uint layer);

  /* Like collision_right except provide a layer. */
  tilecollision@ collision_right_ex(float x1, float y1, float x2, float y2, uint layer);

  /* Get all the entity collisions of a given type in the rectangle. 'type'
   * should be an element from the col_type enum at the bottom of this
   * documentation. The return value is the number of collisions found. Use
   * get_entity_collision_index to query the index'th result. */
  int get_entity_collision(float top, float bottom, float left, float right, uint type);

  /* Returns the index'th entity from the last `get_..._collision` call. */
  entity@ get_entity_collision_index(uint index);

  /* Convenience method for get_entitiy_collision_index that tries to return a
   * controllable. */
  controllable@ get_controllable_collision_index(uint index);

  /* Convenience method for get_entitiy_collision_index that tries to return a
   * dustman. */
  dustman@ get_dustman_collision_index(uint index);

  /* Convenience method for get_entitiy_collision_index that tries to return a
   * hitbox. */
  hitbox@ get_hitbox_collision_index(uint index);

  /* Convenience method for get_entitiy_collision_index that tries to
   * return a script trigger. */
  scripttrigger@ get_scripttrigger_collision_index(uint index);

  /* Convenience method for get_entitiy_collision_index that tries to
   * return a script enemy. */
  scriptenemy@ get_scriptenemy_collision_index(uint index);

  /* Get all the prop collisions within the query rectangle. The return value is
   * the number of prop collisions detected. Use get_prop_collision_index to
   * query the index'th result. */
  int get_prop_collision(float top, float bottom, float left, float right);

  /* Returns the index'th prop from the last `get_..._collision` call. */
  prop@ get_prop_collision_index(uint index);

  /* Override the default stream sizes. Stream sizes are measured in segments
   * (16x16 tile squares). The streaming area is a square region of segments
   * with side length given by the stream size.
   *
   * Constraints:
   *   `8 <= step_size <= load_size <= 256`
   *
   * Arguments:
   *   :load_size: Controls when entities are loaded into memory and
   *     written back to persist.
   *
   *   :step_size: Controls when an entitie's logic will be executed.
   */
  void override_stream_sizes(int load_size, int step_size);

  /* Returns the number of combo breaks that have been recorded for the current
   * replay. This translates to finess scores as 0=S, 1=A, 2-3=B, 4-5=C, 6+=D */
  int combo_break_count();

  /* Set the current combo break count. */
  void combo_break_count(int combo_break_count);

  /* Add a prop into the scene to be rendered each frame. */
  void add_prop(prop@ prop);

  /* Remove a prop from the scene. */
  void remove_prop(prop@ prop);

  /* Add an entity to the scene to be step'ed and drawn. */
  void add_entity(entity@ entity);

  /* Add an entity to the scene to be step'ed and drawn. 'persist' indicates if
   * the entity should be saved and loaded using the checkpoint system. */
  void add_entity(entity@ entity, bool persist);

  /* Remove an entity from the scene. */
  void remove_entity(entity@ entity);

  /* Access the visibility of each layer. */
  bool layer_visible(uint layer);

  void layer_visible(uint layer, bool visible);

  /* Access the scaling factor of the layer. 1.0 is the standard foreground
   * scale with lower values being used for the background. */
  float layer_scale(uint layer);

  void layer_scale(uint layer, float scale);

  /* Reset the render order of the layers to the default. */
  void reset_layer_order();

  /* Swap the rendering order of two layers. Note that this only changes the
   * order that draw commands are applied and does not affect other layer
   * attributes like fog colour or scale.
   *
   * Note that layer order is not persisted across checkpoints. It is up to
   * the script to set the layer ordering appropriately after a checkpoint is
   * loaded.
   */
  void swap_layer_order(uint layer1, uint layer2);

  /* Get the render position of a layer. Normally this is just the layer index
   * itself unless swap_layer_order has been used.
   */
  uint get_layer_position(uint layer);

  /* Draw a rectangle in the world scene's coordinates. colour is an ARGB value
   * in big endian byte order (alpha is the high byte). */
  void draw_rectangle_world(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float rotation, uint colour);

  /* Like draw rectangle except a blur shader is used. */
  void draw_glass_world(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float rotation, uint colour);

  /* Draws a gradient to the screen like how the background is drawn. */
  void draw_gradient_world(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, uint c00, uint c10, uint c11, uint c01);

  /* Deprecated, use draw_line_world instead. */
  void draw_line(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float width, uint colour);

  /* Draws a line between the two points. */
  void draw_line_world(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float width, uint colour);

  /* Generic call to draw an arbitrary quadralateral. Specify points in counter
   * clockwise order. Glass is not actually supported and so is_glass is
   * currently ignored.
   *
   * The engine draws quads under the hood by drawing two triangles between
   * points (1, 2, 3) and points (1, 3, 4).  Note that this means that the color
   * at points 1 and 3 bleed into both halfs while the colors at points 2 and 4
   * are restricted to just one half.
   */
  void draw_quad_world(uint layer, uint sub_layer, bool is_glass, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, uint c1, uint c2, uint c3, uint c4);

  /* Analagous draw routines for the hud coordinate space. To scripts the hud is
   * a 1600 by 900 pixel rectangle centered at the origin. */
  void draw_rectangle_hud(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float rotation, uint colour);

  void draw_glass_hud(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float rotation, uint colour);

  void draw_gradient_hud(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, uint c00, uint c10, uint c11, uint c01);

  void draw_line_hud(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float width, uint colour);

  /* Generic call to draw an arbitrary quadralateral. Specify points in counter
   * clockwise order. is_glass is not supported and is ignored.
   *
   * The engine draws quads under the hood by drawing two triangles between
   * points (1, 2, 3) and points (1, 3, 4).  Note that this means that the color
   * at points 1 and 3 bleed into both halfs while the colors at points 2 and 4
   * are restricted to just one half.
   */
  void draw_quad_hud(uint layer, uint sub_layer, bool is_glass, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, uint c1, uint c2, uint c3, uint c4);

  /* Prevent the normal score overlay (including the combo, combo-meter, and
   * time displays) from rendering. */
  void disable_score_overlay(bool disable_overlay);

  /* See https://gist.github.com/msg555/46f46b8b943ee93393a0a192c7703c57
   * for a list of sound and stream names to use. */
  audio@ play_sound(string name, float x, float y, float volume, bool loop, bool positional);

  /* 'soundGroup' determines which global volume slider to apply to this sound.
   * 1 for music, 2 for ambience, and anything else is considered a sound
   * effect. */
  audio@ play_stream(string name, uint soundGroup, float x, float y, bool loop, float volume);

  /* Play a stream that was loaded in using the script.build_sounds() callback.
   */
  audio@ play_script_stream(string name, uint soundGroup, float x, float y, bool loop, float volume);

  /* Play a sound that will continue to play after a reset, similar to in game music.
   * If the specified audio is already playing nothing will happen.
   * script_sound indicates if the sound to be played is a script, or built in sound.
   * Note that calling audio.stop() on the returned audio won't automatically remove it from
   * the script music registry, use scene.stop_persistent_stream instead.
   */
  audio@ play_persistent_stream(string name, uint sound_group, bool loop, float volume, bool script_sound);

  /* Returns true if the specified persistent stream was stopped.
   */
  bool stop_persistent_stream(string name);

  /* Returns the specified persitent sound if it is playing, or null otherwise. */
  audio@ get_persistent_stream(string name);

  /* Overrides the built in sound named "sound" with "override_sound".
   * Any time the game tries to play that sound, the override will be played instead.
   * script_sound indicates whether an embedded, or built in sound will be used for the override */
  void override_sound(string sound, string override_sound, bool script_sound);

  /* Returns true if the sound has been overridden */
  bool has_sound_override(string sound);

  /* Returns the override for the specified sound, or an empty string */
  string get_sound_override(string sound);

  /* Returns true if the specified override uses a script sound */
  bool is_sound_override_script(string sound);

  /* Clears the specified sound overrides if there is one */
  void clear_sound_override(string sound);

  /* Clears all sound overrides */
  void clear_sound_overrides();

  /* Insert a new collision object into the scene. */
  collision@ add_collision(entity@ e, float top, float bottom, float left, float right, uint32 collision_type);

  /* Returns the x coordinate of the mouse in the hud coordinate space. If scale
   * is set to true will auto scale the coordinates to simulate a 1600-900
   * screen size. Will range between -width/2 and width/2.
   */
  float mouse_x_hud(int player, bool scale);

  /* Equivalent to mouse_x_hud(player, true) */
  float mouse_x_hud(int player);

  /* Returns the y coordinate of the mouse in the hud coordinate space. If scale
   * is set to true will auto scale the coordinates to simulate a 1600-900
   * screen size. Will range between -height/2 and height/2.
   */
  float mouse_y_hud(int player, bool scale);

  /* Equivalent to mouse_y_hud(player, true) */
  float mouse_y_hud(int player);

  /* Returns the x coordinate of the mouse for the given player's camera in the
   * given layer. */
  float mouse_x_world(int player, int layer);

  /* Returns the y coordinate of the mouse for the given player's camera in the
   * given layer. */
  float mouse_y_world(int player, int layer);

  /* Return the current HUD screen width in pixels. If scale is true this
   * always returns 1600. */
  float hud_screen_width(bool scale);

  /* Return the current HUD screen height in pixels. If scale is true this
   * always returns 900. */
  float hud_screen_height(bool scale);

  /* Returns the mouse state for the given player as a bitmask. See the below
   * table for what button each bit corresponds to.
   *
   * Arguments:
   *   :player: the player to get input for
   *
   * Bitmask:
   *   :1: wheel up
   *   :2: wheel down
   *   :4: left click
   *   :8: right click
   *   :16: middle click
   */
  int mouse_state(int player);

  /* Trigger the level to be ended. Note that the replay won't actually end
   * until the next frame if the frame has already begun. (x, y) are the respawn
   * location if the player dies. */
  void end_level(float x, float y);

  /* Access the plugin score used to add an extra criteria for ranks on
   * leaderboards (only) when plugins are used. Lower scores rank better.
   * The Score leaderboard is ranked by
   * (completion, plugin_score, finesse, time) while the Time leaderboard is
   * ranked by (plugin_score, time, completion + finesse).
   *
   * The score is restricted to the rank [0, 1000].
   *
   * Note that this field does nothing when the player isn't using a plugin. */
  int plugin_score();

  void plugin_score(int plugin_score);

  /* Create a new effect based off a sprite animation. */
  entity@ add_effect(string sprite_set, string sprite_name, float x, float y, float rotation, float scale_x, float scale_y, float frame_rate);

  /* Create a new effect based off a sprite animation that follows an entity.
   *
   * follow_x indicates that the effect x coordinate should be computed as
   * follow.x + x. follow_y means similar for y coordinates. If follow_x and
   * follow_y are both false then this behaves the same as add_effect.
   */
  entity@ add_follow_effect(string sprite_set, string sprite_name, float x, float y, float rotation, float scale_x, float scale_y, float frame_rate, entity@ follow, bool follow_x, bool follow_y);

  /* Writes the total initial amount of filth, dustblocks, and enemy life
   * in the level to the passed variables.
   */
  void get_filth_level(int &out filth, int &out filth_block, int &out enemy);

  /* Writes the current amount of filth, dustblocks, and enemy life
   * in the level to the passed variables.
   */
  void get_filth_remaining(int &out filth, int &out filth_block, int &out enemy);

  /* The time warp field can be used to dilate time. e.g. a time_warp of
   * 0.5 will make the world scene run in half time.
   */
  float time_warp();

  void time_warp(float time_warp);

  /* Returns the user ID currently logged into the client. If the user has
   * not logged in successfully this will return 0. If the user has logged in as
   * guest this will return 19998. Otherwise this will be the user's account
   * ID as can be seen on dustkid.
   */
  int user_id();

  /* Like :ref:`scene:user_id<method-scene-user_id>` except returns the user ID
   * attached to a specific player. This is relevant during gameplay where each
   * player may be controlled by a different client logged in with a different
   * ID.
   */
  int player_user_id(uint player);

  /* This returns the username field attached to the replay. This is the same
   * as the text displayed when watching a replay. If no replay is active this
   * will return an empty string.
   */
  string replay_username();

  /* Clear any ghosts currently loaded. This is meant only to help hide replays
   * during special events.
   */
  void clear_ghosts();

}

class rectangle {
  float top();

  void top(float _top);

  float bottom();

  void bottom(float _bottom);

  float left();

  void left(float _left);

  float right();

  void right(float _right);

  float get_width();

  float get_height();

}

/* Represents a ray cast result. */
class raycast {
  /* Returns true if the ray cast hit a tile. */
  bool hit();

  /* Returns the (tile) coordinates of the hit tile. */
  int tile_x();

  int tile_y();

  /* Returns the coordinates pixel coordinates where the ray actually intersects
   * the tile face. */
  float hit_x();

  float hit_y();

  /* Returns 0-3 indicating the side of the edge hit from
   * top, bottom, left, right in that order. */
  int tile_side();

  /* Returns the angle of hit tile surface. */
  int angle();

}

class tilecollision {
  void reset();

  bool hit();

  void hit(bool is_solid);

  float hit_x();

  void hit_x(float outside_x);

  float hit_y();

  void hit_y(float outside_y);

  float angle();

  int type();

  void type(int type);

}

/* Represents what tile shape and sprite is present at a tile and which edges
 * have collisions. Does not include filth information. */
class tileinfo {
  /* See the notes in the TileShape class at
   * https://github.com/msg555/dustmaker/blob/master/dustmaker/Tile.py
   * for how the `type` parameter maps to the shape of the tile.
   *
   * See C's diagram for an illustration of the different tile types.
   * https://github.com/cmann1/PropUtils/blob/master/files/tiles_reference/TileShapes.jpg
   */
  uint8 type();

  void type(int _type);

  /* Indicates whether a tile is present. The rest of the fields are irrelevant
   * if solid is set to false. */
  bool solid();

  void solid(bool _solid);

  /* Angle is a function of the type 'type'. It indicates the angle
   * that the non-flat edge is oriented. A square tile (type 0) has
   * an angle of 0. */
  int32 angle();

  /* See C's reference on the different available sprite set/tile/palettes
   * available.
   *
   * https://github.com/cmann1/PropUtils/blob/master/tile-data.json
   * https://github.com/cmann1/PropUtils/tree/master/files/tiles_reference
   */
  uint8 sprite_set();

  void sprite_set(int _sprite_set);

  uint8 sprite_tile();

  void sprite_tile(uint8 _sprite_tile);

  uint8 sprite_palette();

  void sprite_palette(uint8 _sprite_palette);

  /* Each tile edge is represented by four bits. These are their meanings from
   * least significant bit to most significant bit.
   *
   * 1 bit - indicates edge "priority"?
   * 2 bit - whether to draw an edge cap on the left/top.
   * 4 bit - whether to draw an edge cap on the right/bottom.
   * 8 bit - indicates whether the edge has collision and can have filth.
   */
  uint8 edge_top();

  void edge_top(uint8 _edge_top);

  uint8 edge_bottom();

  void edge_bottom(uint8 _edge_bottom);

  uint8 edge_left();

  void edge_left(uint8 _edge_left);

  uint8 edge_right();

  void edge_right(uint8 _edge_right);

  /* Returns true if the tile is a dustblock tile. */
  bool is_dustblock();

  /* Set the tile's sprite_tile and sprite_palette parameters to be the
   * dustblock tile type in the given sprite set. */
  void set_dustblock(int _sprite_set);

}

/* Describes the filth or spikes on a tile. */
class tilefilth {
  /* Each tile filth value indicates if and what type of filth or spikes are
   * present on a given face of a tile.  These values should be:
   *
   * 0: no filth/spikes
   * 1-5: dust, leaves, trash, slime, virtual filth
   * 9-13: mansion spikes, forest spikes, cones, wires, virtual spikes
   */
  uint8 top();

  void top(uint8 _top);

  uint8 bottom();

  void bottom(uint8 _bottom);

  uint8 left();

  void left(uint8 _left);

  uint8 right();

  void right(uint8 _right);

}

class camera {
  /* Get the camera type.
   *  | `player` - Free camera.
   *  | `test` - The camera is attached to a path.
   *  | `script` - The camera is being controlled by script. */
  string camera_type();

  /* A flag to disable the normal camera behavior. Set this to true if you wish
   * to manage the camera position and zoom entirely within the script. */
  bool script_camera();

  void script_camera(bool script_camera);

  /* The entity the camera is following. */
  entity@ puppet();

  /* Get the player index for this camera. */
  int player();

  /* The controller mode controls how raw game inputs are converted into
   * intents. ispressed, posedge, negedge each convert the corresponding intent
   * to match the corresponding key's state: whether it's currently pressed, was
   * just pushed, or just released. fall_intent is always 0 with a non-standard
   * controller_mode because there is no corresponding key bind.
   */
  int controller_mode();

  void controller_mode(int controller_mode);

  /* Camera center coordinates. */
  float x();

  void x(float x);

  float y();

  void y(float y);

  /* The prev x/y values are used to interpolate the camera position. If you
   * don't want the camera to move between the new and old camera positions
   * reset these values appropriately. This is not necessary if you use
   * camera.reset(). */
  float prev_x();

  void prev_x(float prev_x);

  float prev_y();

  void prev_y(float prev_y);

  /* Deprecated, use screen height instead.*/
  float zoom();

  void zoom(float zoom);

  /* Access the height of the camera in pixels. */
  float screen_height();

  void screen_height(float screen_height);

  /* Access the width of the camera in pixels. */
  float screen_width();

  void screen_width(float screen_width);

  /* Controls how far ahead the camera looks as a multiplier of the
   * puppet velocity. Defaults to (1, 1)
   * This value is persisted with checkpoints. */
  void set_lookahead(float x, float y);

  void get_lookahead(float &out x, float &out y);

  /* Controls how far ahead the camera looks based on the puppet velocity
   * when attached to a camera path.
   * Higher values result in less of an effect. Defaults to (5, 10)
   * This value is persisted with checkpoints. */
  void set_path_lookahead(float x, float y);

  void get_path_lookahead(float &out x, float &out y);

  /* Controls how far outside of the screen the player must be to break camera
   * as a multiplier of the screen size.
   * Set to 1 for normal behaviour and 0 to disable.
   * This value is persisted with checkpoints. */
  void break_distance(float distance);

  float break_distance();

  /* The rate at which the camera eases from it's current to the target position
   * every frame.
   * This value is persisted with checkpoints. */
  void speed_change(float speed_change);

  float speed_change();

  /* A custom speed added to the camera puppet speed when calculating the
   * look ahead position.
   * This value is persisted with checkpoints. */
  void set_custom_speed(float x, float y);

  void get_custom_speed(float &out x, float &out y);

  /* If set the camera will target this position instead of the puppet
   * position.
   * This value is persisted with checkpoints. */
  void set_position_override(float x, float y);

  void get_target_override(float &out x, float &out y);

  /* Disables the override position, making the camera target the puppet
   * again. */
  void clear_position_override();

  /* Returns true if an override position is set. */
  bool has_position_override();

  /* Forcibly disconnect the camera from the current path. */
  void force_disconnect();

  /* Attempts to connect to the given camera node.
   * Returns true on success. */
  bool try_connect(camera_node@ node);

  /* The current node on the camera path. */
  camera_node@ current_node();

  /* The next node on the camera path. */
  camera_node@ next_node();

  /* Access editor zoom setting.  */
  float editor_zoom();

  void editor_zoom(float editor_zoom);

  /* Get the size of the world layer in the current frame at a given
   * sub_frame position. This accounts for camera animations and should
   * match the sizes used by the game. */
  void get_layer_draw_rect(float sub_frame, int layer, float &out left, float &out top, float &out width, float &out height);

  /* The camera rotation in degrees. */
  float rotation();

  void rotation(float rotation);

  float rotation_prev();

  void rotation_prev(float rotation_prev);

  /* These do the same thing as zoom but allow you to manipulate each axis
   * individually. Negative values are support for axis flips. */
  float scale_x();

  void scale_x(float scale_x);

  float scale_y();

  void scale_y(float scale_y);

  float prev_scale_x();

  void prev_scale_x(float prev_scale_x);

  float prev_scale_y();

  void prev_scale_y(float prev_scale_y);

  /* Add a screen shake. Only works if script_camera is false, otherwise you
   * need to simulate your own screen shake. */
  void add_screen_shake(float x, float y, float dir, float force);

  /* Get the current camera fog colours. */
  fog_setting@ get_fog();

  /* Change the fog colour. fog_time controls how long the transition time
   * from the current fog colour to this updated colour should take measured
   * in seconds. */
  void change_fog(fog_setting@ fog, float fog_time);

}

class camera_node : entity {
  /* The size of this node's area. */
  int width();

  void width(int width);

  /* The screen height for this node. */
  int screen_height();

  void screen_height(int height);

  /* This camera node's type.
   * See :ref:`enum node_types` for a list of possible values. */
  int type();

  void type(int type);

  /* Gets the id of the camera node connected to this one at the given
   * edge index. */
  uint connected_node_id(uint index);

  void connected_node_id(uint index, uint node_id);

  /* Gets the interest width for the given edge index. */
  int interest_width(uint index);

  void interest_width(uint index, int width);

  /* Gets the connected node id and interest width for the given edge index. */
  void get_edge(uint index, uint &out node_id, int &out intereset_width);

  void set_edge(uint index, uint node_id, int intereset_width);

  /* Gets length and angle (in degrees) of the control/tolerance handle. */
  void get_control(uint index, float &out length, float &out angle);

  void set_control(uint index, float length, float angle);

  /* Adds a connection to node_id. This may reorder the list of connected nodes.
   * If update_control is false control handle angles will not be recalculated.
   * Returns true if a new connection was added. */
  bool connect_node(uint node_id, bool update_control);

  bool connect_node(uint node_id);

  /* Removes node_id from the list of connected nodes.
   * This may reorder the list of connected nodes.
   * If update_control is false control handle angles will not be recalculated.
   * Returns true if a connection was removed. */
  bool disconnect_node(uint node_id, bool update_control);

  bool disconnect_node(uint node_id);

  /* Removes this node from a neighbouring node's connection list.
   * Returns true if a connection was removed. */
  bool disconnect_from_node(uint node_id);

  /* Removes all connections to and from this node.
   * If update_control is false control handle angles will not be recalculated
   * for neighbouring nodes. */
  void disconnect_all(bool update_control);

  void disconnect_all();

  /* Removes the connected edge at index.
   * If update_control is set this will also recalculate control handle
   * angles. */
  void remove_edge(int index, bool update_control);

  void remove_edge(int index);

  /* Reorders the list of connected nodes based on their relative angle. */
  void reorder_edges(bool update_control);

  /* Recalculate all control handle angles. */
  void update_control();

  /* The number of connections this node has. */
  int num_edges();

  /* The number of control handles this node has.
   * This should be 0 when not connected to any nodes, 2 for a node with a
   * single connection, or match `num_edges` for a node with 2 or more
   * connections. */
  int num_controls();

}

/* Represents a collision hitbox used throughout the game engine. Collisions are
 * made up of a collision type, a hitbox, and an entity. Collisions are used
 * (e.g. when you attack an area) by querying all collision hitboxes of a certain
 * type that intersect with a query rectangle (see scene.get_entity_collision)
 * and returning the entities associated with each intersecting collision.
 *
 * Most enemies have two collisions associated with them. The base collision is
 * used to detect tile collisions and when the entity is clicked in the editor.
 * The hit collision is used to detect when an enemy is attacked. */
class collision {
  /* Access the hitbox of the collisio. */
  void rectangle(float top, float bottom, float left, float right);

  void rectangle(rectangle@ rect, float x_offset, float y_offset);

  rectangle@ rectangle();

  /* Access the collision type of this collision. See col_type for predefined
   * types. New values may be used for custom purposes as well. */
  uint32 collision_type();

  void collision_type(uint32 collision_type);

  /* Remove the collision from the scene. This collision will no longer be
   * picked up by calls to get_entity_collision. */
  void remove();

  /* Access the entity associated with this collision. */
  void entity(entity@ e);

  entity@ entity();

}

class audio {
  void stop();

  bool is_playing();

  float volume();

  void volume(float volume);

  float time_scale();

  void time_scale(float time_scale);

  void set_position(float x, float y);

  bool positional();

  void positional(bool positional);

}

class entity {
  /* Reset the entity state to its defaults. */
  void reset();

  /* Returns true if the underlying entity objects point to the same object.
   * This is to help deal with the issue of different entity handles pointing to
   * the same entity object in the scene. */
  bool is_same(entity@ obj);

  bool is_same(controllable@ obj);

  bool is_same(dustman@ obj);

  message@ metadata();

  /* Returns the entities' sprite object. */
  sprites@ get_sprites();

  void set_sprites(sprites@ obj);

  /* Returns the type name of the entity. This is the same string that can
   * be passed to create_entity to make an object of the same type. */
  string type_name();

  varstruct@ vars();

  /* Recast this object as an entity. Unfortunately with the way the API types
   * are setup a controllable object cannot be casted to an entity using
   * the normal cast<T>() operator. */
  entity@ as_entity();

  /* Attempt to recast this object as a hittable. Returns null if the
   * entity is not a hittable. */
  hittable@ as_hittable();

  /* Attempt to recast this object as a controllable. Returns null if the
   * entity is not a controllable. */
  controllable@ as_controllable();

  /* Attempt to recast this object as a dustman object. Returns null if
   * the entity is not a dustman object. */
  dustman@ as_dustman();

  /* Attempt to recast this object as a hitbox object. Returns null if
   * the entity is not a hitbox object. */
  hitbox@ as_hitbox();

  /* Attempt to recast this object as an effect object. Returns null if
   * the entity is not a effect object. */
  effect@ as_effect();

  /* Attempt to recast this object as a filth ball object. Returns null if
   * the entity is not a filth  object. */
  filth_ball@ as_filth_ball();

  /* Attempt to recast this object as a scripttrigger object. Returns null if
   * the entity is not a scripttrigger object. */
  scripttrigger@ as_scripttrigger();

  /* Attempt to recast this object as a scriptenemy object. Returns null if
   * the entity is not a scriptenemy object. */
  scriptenemy@ as_scriptenemy();

  /* Attempt to recast this object as a scriptenemy object. Returns null if
   * the entity is not a scriptenemy object. */
  camera_node@ as_camera_node();

  /* Return the ID associated with this entity that can be used with the
   * entity_by_id() function. Non-persistant entities (i.e. the player
   * entities) will have an id of 0 and cannot be found with entity_by_id(). */
  uint id();

  /* Has this entity been removed from the scene. */
  bool destroyed();

  /* The position of the entity. For most entities the position is the bottom
   * center of their collision rectangle. These functions will automatically
   * adjust the base and hit collisions associated with this entity. */
  float x();

  void x(float x);

  float y();

  void y(float y);

  void set_xy(float x, float y);

  /* A shortcut method to calculate the centre of this entity's hitbox, which
   * for some entities may not be the same as its origin/position. */
  void centre(float &out x, float &out y);

  float centre_x();

  float centre_y();

  /* Sets this entity's position so that the centre of its hitbox will be
   * positioned at `x`, `y` */
  void set_centre(float x, float y);

  /* The rotation of the entity in degrees. This should be in the interval
   * [-180, 180]. */
  float rotation();

  void rotation(float rot);

  /* The layer that the entity should be drawn in. */
  int layer();

  void layer(int layer);

  /* The direction the entity is facing. Should be -1 for left or 1 for right.
   * If this is a controllable entity and the attack state is not
   * attack_type_idle (i.e. non-zero) then the controllable will temporarily
   * be facing the direction given by attack_face() instead. */
  int face();

  void face(int face);

  /* The palette of sprites to use. Typically this should just be set to 1 as
   * most entities don't have alternative palettes for their animations. */
  int palette();

  void palette(int palette);

  /* Changes the perceived game speed for the entity. */
  float time_warp();

  void time_warp(float time_warp);

  /* Returns the collision rectangle used to select the entity in the editor. */
  collision@ base_collision();

  rectangle@ base_rectangle();

  void base_rectangle(float top, float bottom, float left, float right);

  void base_rectangle(rectangle@ rect);

  /* Send a message to the entity. Currently, scripttrigger and scriptenemy
   * entities are the only entities that can do anything with the message. */
  void send_message(string id, message@ msg);

  /* The sprite name currently being rendered for this entity. */
  string sprite_index();

  void sprite_index(string spr_index);

}

class hittable : entity {
  /* Returns the x/y component of the velocity measured in pixels per second. */
  float x_speed();

  float y_speed();

  /* Sets the velocity using x/y components. */
  void set_speed_xy(float x_speed, float y_speed);

  /* Returns a copy of the hurtbox rectangle for the entity. */
  rectangle@ hurt_rect();

  /* Access the scale of the entity. A scale of 2.0 means double the usual size.
   * A scale of 0.5 means half the usual size. If 'animate' is true the scale
   * changes will gradually take affect. */
  float scale();

  void scale(float scale);

  void scale(float scale, bool animate);

  /* Gives the initial life associated with this entity. This also usually
   * corresponds to how much dust the enemy contributes toward completion
   * score calculations. */
  int life_initial();

  /* Access the number of hits remaining on this enemy. Setting the life
   * negative will not destroy the enemy until it is hit again. A few
   * controllables don't make use of this field (e.g. hittable_apple). */
  int life();

  void life(int life);

  /* Returns the current hitbox controller for this entity. This may be null
   * if the controllabe isn't attacking. The hitbox object associated with each
   * controllable is recreated with each attack. */
  hitbox@ hitbox();

  /* Returns the hurt collision object for this controlable. */
  collision@ hit_collision();

  rectangle@ hit_rectangle();

  void hit_rectangle(float top, float bottom, float left, float right);

  void hit_rectangle(rectangle@ rect);

  /* Access the team of the hittable. See the team_types enum for predefined
   * values. Normally entities will only hit/target entities of the opposite
   * team. */
  int team();

  void team(int team);

  /* Access the freeze frame timer for this entity. This timer usually runs
   * at 24 units/s. */
  float freeze_frame_timer();

  void freeze_frame_timer(float freeze_frame_timer);

  /* Set a callback when the entity is hit. The callback should have the
   * signature "void func_name(controllable@ attacker, controllable@ attacked,
   * hitbox@ attack_hitbox, int arg)".
   * hittable@ is allowed as the attacker/attacked may not be controllables
   * e.g. in the case of quills.
   * The 'arg' value passed to on_hit_callback will match the 'arg' parameter
   * passed to the callback. */
  void on_hit_callback(callback_base@ base_obj, string callback_method, int arg);

  /* Set a callback when the entity is hurt. The callback should have the
   * signature "void func_name(controllable@ attacked, controllable@ attacker,
   * hitbox@ attack_hitbox, int arg)".
   * hittable@ is allowed as the attacker/attacked may not be controllables
   * e.g. in the case of quills.
   * The 'arg' value passed to on_hurt_callback will match the 'arg' parameter
   * passed to the callback. */
  void on_hurt_callback(callback_base@ base_obj, string callback_method, int arg);

}

class controllable : hittable {
  /* The prev x/y values are used to interpolate the entity position. */
  float prev_x();

  void prev_x(float prev_x);

  float prev_y();

  void prev_y(float prev_y);

  /* Sets the velocity using x/y components. */
  void set_speed_xy(float x_speed, float y_speed);

  /* Returns magnitude of the velocity. */
  float speed();

  /* Returns the direction of the velocity vector. Right is 90, Left is -90,
   * Up is 0, Down is -180 or 180. */
  float direction();

  /* Sets the velocity using polar components. */
  void set_speed_direction(float speed, int direction);

  /* Returns a copy of the collision rectangle for the entity. */
  rectangle@ collision_rect();

  /* Returns the current state of the entity. See the 'state_types' enum at the
   * end of this documentation for details on the different states names.
   * The majorify of the states are only used by dustman. */
  int state();

  void state(int state);

  /* The sprite name currently being rendered for this entity. */
  string attack_sprite_index();

  void attack_sprite_index(string attack_spr_index);

  /* The state timer for this entity. This tracks where the entity is in
   * the state animation. */
  float state_timer();

  void state_timer(float state_timer);

  /* The stun timer for this entity. This counts down to 0 which ends the stun
   * animation. */
  float stun_timer();

  void stun_timer(float stun_timer);

  /* The attack state for this entity. See 'attack_types' at the bottom of this
   * documentation for the attack state types. */
  int attack_state();

  void attack_state(int attack_state);

  /* The timer that keeps track of how long the attack has been active. */
  float attack_timer();

  void attack_timer(float attack_timer);

  /* The direction the controllable is facing while attack state is not
   * attack_type_idle. */
  int attack_face();

  void attack_face(int attack_face);

  /* Indicates what direction the entity wants to move in the x direction. -1
   * for left, 0 for neutral, 1 for right. */
  int x_intent();

  void x_intent(int x_intent);

  /* Indicates what direction the entity wants to move in the y direction. -1
   * for up, 0 for neutral, 1 for down. */
  int y_intent();

  void y_intent(int y_intent);

  /* 0 indicates taunt not pressed. 1 indicates taunt is pressed. 2 indicates
   * taunt is pressed and the intent has been used. */
  int taunt_intent();

  void taunt_intent(int taunt_intent);

  /* 0 indicates no heavy intended. 10 indicates heavy pressed. When heavy is
   * released and the intent was never used it counts down from 10 to 0 until
   * the intent ends up being used or it hits 0. 11 indicates heavy is pressed
   * and the intent has been used. */
  int heavy_intent();

  void heavy_intent(int heavy_intent);

  /* Functions the same as heavy_intent() */
  int light_intent();

  void light_intent(int light_intent);

  /* 0 indicates no dash key press. 1 indicates the dash key pushed this frame.
   * 2 indicates the dash key pushed this frame and the intent has been used. */
  int dash_intent();

  void dash_intent(int dash_intent);

  /* Same as taunt_intent() */
  int jump_intent();

  void jump_intent(int jump_intent);

  /* Same as dash_intent() */
  int fall_intent();

  void fall_intent(int fall_intent);

  /* Returns true if the controllable is in contact with the corresponding
   * surface type. */
  bool ground();

  void ground(bool ground);

  bool roof();

  void roof(bool roof);

  bool wall_left();

  void wall_left(bool wall_left);

  bool wall_right();

  void wall_right(bool wall_right);

  /* If the corresponding surface flag is set then these fields contain the
   * angle of the surface the entity is touching. */
  int ground_surface_angle();

  int roof_surface_angle();

  int left_surface_angle();

  int right_surface_angle();

  float run_max();

  void run_max(float run_max);

  float run_start();

  void run_start(float run_start);

  float run_accel();

  void run_accel(float run_accel);

  float idle_fric();

  void idle_fric(float idle_fric);

  float skid_fric();

  void skid_fric(float skid_fric);

  /* Get/set the friction value used by some enemies to control air, turn,
   * or attack friction. */
  float fric();

  void fric(float fric);

  /* Change which ground surface angles this entity considers slopes, or slants.
   * Slopes default to 45, and slants to 26.
   * Required to allow non-45 degress slope sliding and for the player sprite to
   * automatically rotate to match the ground angle. */
  void set_ground_angles(int slope_min, int slope_max, int slant_min, int slant_max);

  /* Change which roof surface angles this entity considers slopes, or slants.
   * Slopes defaults to 135, and slants to 154.
   * Required for the player sprite to automatically rotate to match the
   * ceiling angle. */
  void set_roof_angles(int slope_min, int slope_max, int slant_min, int slant_max);

  /* Change which wall surface angles this entity considers down and up facing
   * slants.
   * Down facing defaults to 116, and up facing to 64.
   * Required or wall angles outside of the range
   * slant_up_min < 90 < slant_down_max
   * won't work, even if the custom collision handler returns a collision. */
  void set_wall_angles(int slant_down_min, int slant_down_max, int slant_up_min, int slant_up_max);

  /* Performs the default collision check on the specified side used by
   * all entities and writes the result into t.
   *
   * `moving` indicates if the collision should compensate for movement of
   * the hitbox over the previous subframe (i.e. for checking for collisions
   * between subframes).
   *
   * `snap_offset` indicates an additional offset outside of the collision
   * for the entity to look for a surface used in e.g. wall snap jumps/dashes.
   *
   * Returns true if the collision hits anything.
   */
  bool check_collision(tilecollision@ t, int side, bool moving, float snap_offset);

  /* Overrides the default tile collision checking for this entity.
   * Setting `base_obj` to null will clear the handler.
   *
   * Arguments:
   *   :base_obj: The object the callback will be invoked on.
   *
   *   :callback_method: The name of the function to invoke.
   *   :arg: An opaque value to be passed back to the callback when invoked.
   *
   * The callback should have the signature: ::
   *
   *   void func_name(controllable@ ec, tilecollision@ tc, int side, bool moving, float snap_offset, int arg)
   *
   * Callback Arguments:
   *   :ec: The entity to check collisions for
   *   :tc: The tile collision object to write results to
   *   :side: which surface type to check for collisions, see the `side_types` enum.
   *   :moving: Indicates if the collision should compensate for movement over the last subframe.
   *   :snap_offset: Indicates an extra offset to look for collisions used in e.g. snap jumps/dashes
   *   :arg: The same value that was passed when registering the callback.
   *
   * Use the provided tilecollision object to return the results of the custom
   * collision.
   *
   * Collision Results:
   *   :hit(): Set to true to indicate a collision happened
   *   :type(): Sets the surface angle of the collision (angle() is not used)
   *   :hit_x/y(): The position of the collision
   *
   * Calling :ref:`controllable::check_collision<method-controllable-check_collision>` can be used to
   * perform the default tile collision handling when needed.
   * */
  void set_collision_handler(callback_base@ base_obj, string callback_method, int arg);

  /* Overrides the default surface texture type lookup for entity.
   * Setting base_obj to null will clear the handler.
   *
   * The callback should have the signature:
   * void func_name(controllable@, texture_type_query@, int)
   *
   * See texture_type_query for details.
   * */
  void set_texture_type_handler(callback_base@ base_obj, string callback_method, int arg);

  /* Stuns the controllable. This does not break combo. */
  void stun(float stun_x_speed, float stun_y_speed);

  /* The game offsets the rendering of the sprites when on some surfaces or
   * when stunned (and perhaps more). Added draw_offset_x() and draw_offset_y()
   * to the entity's actual coordinates if you wish to compensate for this. Note
   * that stun offsets are RNG and shouldn't affect game play.
   */
  float draw_offset_x();

  float draw_offset_y();

  /* Set the drawing offsets. This does not include stun offsets.
   */
  void draw_offset_x(float x_offset);

  void draw_offset_y(float y_offset);

  /* Return the player index of this controllable entity. If the entity is not
   * associated with a player returns -1. This is the reverse function of
   * controller_entity(player). */
  int player_index();

  /* Controls the type of dust this entity spread.
   * See :ref:`enum filth_types` for a list of possible values. */
  int filth_type();

  void filth_type(int filth_type);

  /* Controls the type of emitter created when hit.
   * If set to 'filth_type_default' the `controllable.filth_type` value will be used instead. */
  int emitter_filth_type();

  void emitter_filth_type(int filth_type);

}

class dustman : controllable {
  float run_accel_over();

  void run_accel_over(float run_accel_over);

  float dash_speed();

  void dash_speed(float dash_speed);

  float slope_slide_speed();

  void slope_slide_speed(float slope_slide_speed);

  float slope_max();

  void slope_max(float slope_max);

  /* Internally this value is not used anywhere. */
  float land_fric();

  void land_fric(float land_fric);

  float roof_fric();

  void roof_fric(float roof_fric);

  float skid_threshold();

  void skid_threshold(float skid_threshold);

  float jump_a();

  void jump_a(float jump_a);

  float hop_a();

  void hop_a(float hop_a);

  float fall_max();

  void fall_max(float fall_max);

  float fall_accel();

  void fall_accel(float fall_accel);

  float hover_accel();

  void hover_accel(float hover_accel);

  float heavy_fall_threshold();

  void heavy_fall_threshold(float heavy_fall_threshold);

  float hover_fall_threshold();

  void hover_fall_threshold(float hover_fall_threshold);

  float hitrise_speed();

  void hitrise_speed(float hitrise_speed);

  float di_speed();

  void di_speed(float di_speed);

  float di_speed_wall_lock();

  void di_speed_wall_lock(float di_speed_wall_lock);

  float di_move_max();

  void di_move_max(float di_move_max);

  float wall_slide_speed();

  void wall_slide_speed(float wall_slide_speed);

  float wall_run_length();

  void wall_run_length(float wall_run_length);

  float roof_run_length();

  void roof_run_length(float roof_run_length);

  float attack_force_light();

  void attack_force_light(float attack_force_light);

  int combo_count();

  void combo_count(int combo_count);

  int skill_combo();

  void skill_combo(int skill_combo);

  int skill_combo_max();

  void skill_combo_max(int skill_combo_max);

  float combo_timer();

  void combo_timer(float combo_timer);

  int total_filth();

  /* Query/set the number of air charges the player has. */
  int dash();

  void dash(int dash);

  /* Query/set the maximum number of air charges the player has. */
  int dash_max();

  void dash_max(int dash_max);

  /* Access the speed required to clear dustblocks when falling, as a
   * percentage of 'fall_max'. */
  float dustblock_clear_speed();

  void dustblock_clear_speed(float speed);

  /* Get/set the rectangle relative to the player centre used to clear
   * dustblocks when falling. */
  rectangle@ dustblock_clear_rect();

  void dustblock_clear_rect(rectangle@ rect);

  void dustblock_clear_rect(float top, float bottom, float left, float right);

  void get_dustblock_clear_rect(float &out top, float &out bottom, float &out left, float &out right);

  /* Should be one of dustman, dustgirl, dustkid, dustworth, dustwraith,
   * leafsprite, trashking, slimeboss. Optionally add 'v' to the start of the
   * name to make it a virtual character. Using the string "default" will return
   * the character choice to what the player initially selected. */
  string character();

  void character(string character);

  /* Controls the type of dust spread when attacking.
   * If set to 'filth_type_default' the :ref:`controllable::filth_type<method-controllable-filth_type>` value will be used instead. */
  int attack_filth_type();

  void attack_filth_type(int filth_type);

  /* Normally dustman entities that aren't attached to a camera are taken over
   * by the default AI implementation. Set this flag to disable this behavior.
   */
  bool ai_disabled();

  void ai_disabled(bool ai_disabled);

  /* Determines if the player is considered 'dead'. This is useful if you
   * disable auto respawning or want to make the player invincible. */
  bool dead();

  void dead(bool dead);

  /* Disable the player from respawning on death automatically. */
  bool auto_respawn();

  void auto_respawn(bool auto_respawn);

  /* Simulate the player hitting a death zone if as_spikes=false, otherwise
   * simulate them hitting spikes. Sets the dead flag but doesn't check if it
   * was already set. */
  void kill(bool as_spikes);

  /* Set a callback after every substep of this dustman object.
   * The callback should have the
   * signature "void func_name(dustman@ dm, int arg).
   * The 'arg' value passed to on_subframe_end_callback
   * will match the 'arg' parameter passed to the callback. */
  void on_subframe_end_callback(callback_base@ base_obj, string callback_method, int arg);

}

class hitbox : entity {
  controllable@ owner();

  /* Can be used instead of `owner()` if the owner is not a controllable,
   * e.g. in the case of a quill. */
  hittable@ hittable_owner();

  int damage();

  void damage(int damage);

  int filth_type();

  void filth_type(int filth_type);

  /* Used to indicate that the force from this hitbox should be applied radially
   * outward from the center rather than using the attack_dir. */
  bool aoe();

  void aoe(bool aoe);

  float state_timer();

  void state_timer(float state_timer);

  float activate_time();

  void activate_time(float activate_time);

  float timer_speed();

  void timer_speed(float timer_speed);

  /* Attack freeze frame strength. Controls how long the hit entity is frozen.
   */
  float attack_ff_strength();

  void attack_ff_strength(float attack_ff_strength);

  float parry_ff_strength();

  void parry_ff_strength(float parry_ff_strength);

  float stun_time();

  void stun_time(float stun_time);

  bool can_parry();

  void can_parry(bool can_parry);

  int attack_dir();

  void attack_dir(int attack_dir);

  float attack_strength();

  void attack_strength(float attack_strength);

  int team();

  void team(int team);

  string attack_effect();

  void attack_effect(string attack_effect);

  int effect_frame_rate();

  void effect_frame_rate(int effect_frame_rate);

  bool triggered();

  /* See the hit_outcomes enumeration for possible values.
   * This may change after being triggered (e.g. when the player changes state),
   * in which case `hitbox.triggered_outcome` can be used. */
  int hit_outcome();

  void hit_outcome(int hit_outcome);

  /* Returns the outcome when this hitbox triggered, or ho_unresolved if it has
   * not triggered yet. */
  int triggered_outcome();

  /* Called for each entity this hitbox will potentially hit. Return false to
   * ignore the given entity.
   * The callback should have the signature
   * "bool func_name(hitbox@, hittable@, int)".
   * The 'arg' value passed to on_hit_callback will match the 'arg' parameter
   * passed to the callback. */
  void on_hit_filter_callback(callback_base@ base_obj, string callback_method, int arg);

}

/* Represents a sprite animation that can optionally move or follow the path of
 * an existing entity. */
class effect : entity {
  /* If false the effect will ignore time scale. */
  bool slow_down();

  void slow_down(bool slow_down);

  float scale_x();

  void scale_x(float scale_x);

  float scale_y();

  void scale_y(float scale_y);

  void set_scale(float scale_x, float scale_y);

  float x_speed();

  void x_speed(float x_speed);

  float y_speed();

  void y_speed(float y_speed);

  void set_speed(float x_speed, float y_speed);

  /* If not null then x/y positions of the effect will be interpreted as
   * an offset of the follow_target entity. This can be controlled in each
   * axis by toggling `effect.follow_x` and `effect.follow_y` as well. */
  entity@ follow_target();

  void follow_target(entity@ follow_target);

  /* If not null then the effect will freeze whenever freeze_target is
   * experiencing freeze frames.
   */
  hittable@ freeze_target();

  void freeze_target(hittable@ freeze_target);

  /* If true then the x-coordinate of the follow_target will be followed */
  bool follow_x();

  void follow_x(bool follow_x);

  /* If true then the y-coordinate of the follow_target will be followed */
  bool follow_y();

  void follow_y(bool follow_y);

  void set_follow(bool follow_x, bool follow_y);

  string sprite_set();

  void sprite_set(string sprite_set);

  float state_timer();

  void state_timer(float state_timer);

  int total_frames();

  float frame_rate();

  void frame_rate(float frame_rate);

  int sub_layer();

  void sub_layer(int sub_layer);

  uint colour();

  void colour(uint b_colour);

}

/* Spawned after an enemy is cleaned and calls
 * :ref:`scene::project_tile_filth<method-scene-project_tile_filth>` when the
 * state timer runs out.
 *
 * The top/bottom/left/right flags are calculated automatically based
 * on :ref:`direction<method-filth_ball-direction>`, and
 * :ref:`base_rectangle<method-entity-base_rectangle>` is used for the
 * 'baseWidth' and 'baseHeight` arguments. */
class filth_ball : entity {
  /* Controls the type of dust this entity spreads.
   * See :ref:`enum filth_types` for a list of possible values. */
  int filth_type();

  void filth_type(int filth_type);

  /* See :ref:`scene::project_tile_filth<method-scene-project_tile_filth>`
   * for a description of this value.
   * The top/bottom/left/right flags will automatically be set based on this. */
  int direction();

  void direction(int direction);

  /* See :ref:`scene::project_tile_filth<method-scene-project_tile_filth>`
   *  for a description of this value. */
  float distance();

  void distance(float distance);

  /* When this counts down to zero the filth ball will spread dust and remove
   * itself from the scene. */
  float state_timer();

  void state_timer(float state_timer);

}

/* Represents a generic script-backed trigger. */
class scripttrigger : entity {
  /* Returns the script object backing this trigger. If this object is from a
   * different script than the calling script this will return null instead. */
  trigger_base@ get_object();

  /* Returns the name of the script this trigger comes from. */
  string script_name();

  /* Returns the class name of this trigger within its script. */
  string type_name();

  bool editor_selected();

  /* Access the radius of the activation circle or square around this trigger.
   * If the trigger has a square shape then the square extends width() out in
   * each direction from the center of the trigger, i.e. it's side length is
   * 2*width(). */
  int radius();

  void radius(int radius);

  /* Access if the trigger has a square or circle activation area. */
  bool square();

  void square(bool square);

  /* Access whether the radius is visible in the editor or not. */
  bool editor_show_radius();

  void editor_show_radius(bool show_radius);

  /* Access the size of the trigger handle in the editor. The handle size is how
   * many pixels in each direction the handle should extend, default is 10. */
  int editor_handle_size();

  void editor_handle_size(int handle_size);

  /* Access the colour of the trigger handle when it is selected. */
  uint editor_colour_active();

  void editor_colour_active(uint colour);

  /* Access the colour of the trigger handle when it is not selected. */
  uint editor_colour_inactive();

  void editor_colour_inactive(uint colour);

  /* Access the colour of the activation circle/square. */
  uint editor_colour_circle();

  void editor_colour_circle(uint colour);

  /* Use after changing persistent variables via script to update values in the trigger script panel. */
  void editor_sync_vars_menu();

}

/* Represents a generic script-backed enemy. */
class scriptenemy : controllable {
  /* Returns the enemy object backing this enemy. If this object is from a
   * different script than the calling script this will return null instead. */
  enemy_base@ get_object();

  /* Returns the name of the script this enemy comes from. */
  string script_name();

  /* Returns the class name of this enemy within its script. */
  string type_name();

  bool auto_physics();

  void auto_physics(bool auto_physics);

  /* Use after changing persistent variables via script to update values in the enemy script panel. */
  void editor_sync_vars_menu();

}

/* Represents a set of sprites that can be drawn. Sprites are organized into
 * sprite set files that can be seen in 'content/sprites' and can be added
 * into this sprite object using 'add_sprite_set' call.
 *
 * Each sprite set file has a list of sprite names. These names can be
 * enumerated using get_sprite_count(sprite_set) and
 * get_sprite_name(sprite_set, i).
 *
 * Each sprite name has some number of color palettes (most often it's just 1)
 * which can be counted using get_palette_count(). They also have
 * some number of frames which you can calculate with get_animation_length().
 *
 * You can download all of the sprites from
 * https://www.dropbox.com/s/11pa1cdqhv68etv/sprites.rar?dl=0 although the
 * folder structure doesn't always match the sprite set structure used in game.
 */
class sprites {
  /* Add a sprite set's sprites into this sprites object. After this call we can
   * refer to any contained sprites with
   * draw/get_palette_count/get_animation_length.  Any duplicate sprite
   * names will be overwritten to point to the most recently added sprite set.
   */
  void add_sprite_set(string sprite_set);

  /* Returns the number of frames the named sprite last. */
  int get_animation_length(string sprite_name);

  /* Returns the number of palettes associated with the sprite. Tile sprites are
   * typically the only sprites to have multiple palettes. */
  uint get_palette_count(string sprite_name);

  /* Returns the number of sprites contained in the sprite set. */
  uint get_sprite_count(string sprite_set);

  /* Returns the name of the index'th sprite in sprite_set. */
  string get_sprite_name(string sprite_set, uint index);

  /* Returns the bounding rectangle around the sprite if it were rendered at the
   * origin. */
  rectangle@ get_sprite_rect(string sprite_name, uint32 frame);

  /* Draw a sprite to the world.
   * 1 <= palette <= get_palette_count(spriteName)
   * 0 <= frame < get_animation_length(spriteName)
   *
   * colour is an ARGB colour vector that is multiplied with the actual color
   * values rendered. This does not include color manipulation through fog
   * colours. Setting colour to 0xFFFFFFFF draws the sprite normally with only
   * fog colour applied.
   */
  void draw_world(int layer, int sub_layer, string spriteName, uint32 frame, uint32 palette, float x, float y, float rotation, float scale_x, float scale_y, uint32 colour);

  /* Like draw_world except drawing in the hud. */
  void draw_hud(int layer, int sub_layer, string spriteName, uint32 frame, uint32 palette, float x, float y, float rotation, float scale_x, float scale_y, uint32 colour);

}

class prop {
  /* Return the ID associated with this prop that can be used with the
   * prop_by_id() function. All props that have been added to the scene should
   * have an ID. */
  uint id();

  float x();

  void x(float x);

  float y();

  void y(float y);

  float rotation();

  void rotation(float rotation);

  float scale_x();

  void scale_x(float scale_x);

  float scale_y();

  void scale_y(float scale_y);

  /* C has an excellent reference to find the prop set/group/index for props
   * which can be found at
   * https://github.com/cmann1/PropUtils/tree/master/files/prop_reference. The
   * three numbers listed under each prop correspond to the set, group, and
   * index for that prop. */
  uint prop_set();

  void prop_set(uint prop_set);

  uint prop_group();

  void prop_group(uint prop_group);

  uint prop_index();

  void prop_index(uint prop_index);

  /* The palette for the prop. Most (all?) props only support palette 1. */
  uint palette();

  void palette(uint palette);

  /* The layer to render the prop in. */
  uint layer();

  void layer(uint layer);

  /* The sublayer to render the prop in. */
  uint sub_layer();

  void sub_layer(uint sub_layer);

}

/* Represents a text field used to render text to the screen. */
class textfield {
  /* Access the text to be rendered or measured. */
  string text();

  void text(string text);

  /* Get and set the font used to render the text. See
   * https://pastebin.com/YcNKSXd9 for a list of supported fonts and font
   * sizes. */
  string font();

  uint font_size();

  void set_font(string font, uint font_size);

  /* Set the colour of the text as an ARGB value. */
  uint colour();

  void colour(uint colour);

  /* Set the horizontal alignment when drawing the text. -1 aligns the left of
   * the text to the given x coordinate. 0 centers the text at the given x
   * coordinate. 1 aligns the right of the text to the given x coordinate. */
  int align_horizontal();

  void align_horizontal(int align_h);

  /* Set the vertical alignment when drawing the text. -1 aligns the top of the
   * text to the given y coordinate.  0 centers the text at the given y
   * coordinate. 1 aligns the bottom of the text to the given y coordinate.
   */
  int align_vertical();

  void align_vertical(int align_v);

  /* Measures the width of the text when drawn. */
  int text_width();

  /* Measures the height of the text when drawn. */
  int text_height();

  void draw_world(int layer, int sub_layer, float x, float y, float scale_x, float scale_y, float rotation);

  void draw_hud(int layer, int sub_layer, float x, float y, float scale_x, float scale_y, float rotation);

}

/* A common interface used to represent a variable of any type. */
class varvalue {
  /* Returns the type of the variable.  See 'var_types' at the end of this
   * documentation for details on the different types. */
  int type_id();

  /* Get or set the value. The getter or setter used must match the type_id() of
   * the var. */
  bool get_bool();

  void set_bool(bool val);

  int8 get_int8();

  void set_int8(int8 val);

  int16 get_int16();

  void set_int16(int16 val);

  int32 get_int32();

  void set_int32(int32 val);

  int64 get_int64();

  void set_int64(int64 val);

  float get_float();

  void set_float(float val);

  string get_string();

  void set_string(string val);

  float get_vec2_x();

  float get_vec2_y();

  void set_vec2(float x, float y);

  /* Returns a modifiable handle to the struct pointed to by this var. */
  varstruct@ get_struct();

  /* Returns a modifiable handle to the array pointed to by this var. */
  vararray@ get_array();

}

/* Represents an array of variables. */
class vararray {
  /* Get the i-th value in this array. Returns null for indicies outside the
   * bounds of the array. */
  varvalue@ at(uint32 index);

  /* Returns the element type of this array. */
  int element_type_id();

  /* Returns the size of this array. */
  uint32 size();

  /* Resizes this array. If the array size is extended the new elements are not
   * initialized. */
  void resize(uint32 size);

}

/* Represents a dictionary of string keys to vars. */
class varstruct {
  /* Retrieves a var based on its name. */
  varvalue@ get_var(string var);

  /* Retrieves the index'th var. */
  varvalue@ get_var(uint32 index);

  /* Returns the number of vars in this struct. */
  uint32 num_vars();

  /* Returns the type name of the struct. */
  string type_name();

  /* Returns the name of the index'th var. */
  string var_name(uint32 index);

}

class message {
  int get_int(string key);

  int get_int(string key, int def);

  void set_int(string key, int val);

  bool has_int(string key);

  uint num_int();

  string get_key_int(uint index);

  float get_float(string key);

  float get_float(string key, float def);

  void set_float(string key, float val);

  bool has_float(string key);

  uint num_float();

  string get_key_float(uint index);

  string get_string(string key);

  string get_string(string key, string def);

  void set_string(string key, string val);

  bool has_string(string key);

  uint num_string();

  string get_key_string(uint index);

  entity@ get_entity(string key);

  void set_entity(string key, entity@ e);

  bool has_entity(string key);

  uint num_entity();

  string get_key_entity(uint index);

}

class var_info {
  /* Returns the name of the variable that changed */
  string get_name();

  /* Returns the index that was changed, or -1 if this variable is not an array */
  int get_index();

  /* For nested variables returns the number of variables above this one */
  int get_path_length();

  /* Returns the name of the parent variable at the specified level */
  string get_name(uint index);

  /* Returns the array index of the parent variable at the specified level, or -1 if it is not an array */
  int get_index(uint index);

  /* Convenience method that returns the entire path as a string
   * in the format: "parent_var.parent_var2.var"
   * If include_array_indices is true, arrays in the path will also
   * include an index, e.g. "parent_var[i]" */
  string get_path(bool include_array_indices);

}

/* Certain sounds, e.g. player footstep sounds, check the tiles around the
 * player's feet to determine which sounds to play based on the tile
 * sprite set and index. controllable::set_texture_type_handler can be used
 * to bypass that behaviour and provide a custom texture type.
 *
 * This class is used to pass values back and forth during a texture
 * type callback registered with controllable::set_texture_type_handler.
 * */
class texture_type_query {
  /* The location to check */
  int x();

  /* The location to check */
  int y();

  /* Is the query for a ground surface? */
  bool top_surface();

  string result();

  /* Set this to return the texture type or use an empty string to
   * use the default texture lookup.
   *
   * Valid textures are `"none"`, `"stone"`, `"dirt"`, `"metal"`, `"grass"`
   * `"water"`, `"wood"`, `"carpet"`, or `"poly"`.
   */
  void result(string result);

}

/* A configurable drawing surface that supports affine transformations.
 *
 * The transformaton matrix is stored in the form ::
 *
 *   [x']   [m00 m01 ox] [x]
 *   [y'] = [m10 m11 oy] [y]
 *   [1 ]   [0   0   1 ] [1]
 */
class canvas {
  /* Access whether this canvas draws to the hud or world. */
  bool hud();

  void hud(bool hud);

  /* Access what layer this canvas draws to. */
  int layer();

  void layer(int layer);

  /* Access what sub layer this canvas draws to. */
  int sub_layer();

  void sub_layer(int sub_layer);

  /* These draw routines mirror the existing draw routines using the canvas
   * specific options and transformation. */
  void draw_rectangle(float x1, float y1, float x2, float y2, float rotation, uint colour);

  void draw_glass(float x1, float y1, float x2, float y2, float rotation, uint colour);

  void draw_gradient(float x1, float y1, float x2, float y2, uint c00, uint c10, uint c11, uint c01);

  void draw_line(float x1, float y1, float x2, float y2, float width, uint colour);

  void draw_quad(bool is_glass, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, uint c1, uint c2, uint c3, uint c4);

  void draw_sprite(sprites@ spr, string spriteName, uint32 frame, uint32 palette, float x, float y, float rotation, float scale_x, float scale_y, uint32 colour);

  void draw_text(textfield@ txt, float x, float y, float scale_x, float scale_y, float rotation);

  /* Compute the x/y coordinates of the underlying canvas transform. */
  void transform_point(float x, float y, float &out tx, float &out ty);

  /* Right multiply a generic affine matrix. */
  void multiply(float m00, float m01, float m10, float m11, float ox, float oy);

  /* Left multiply a generic affine matrix. */
  void multiply_left(float m00, float m01, float m10, float m11, float ox, float oy);

  /* Each transformation option has a right (default) and left multiply variant.
   * A right multiply transformation is applied to the source coordinates, a
   * left multiply is applied to the transformed coordinates.
   */
  void scale(float scale_x, float scale_y);

  void scale_left(float scale_x, float scale_y);

  void translate(float ox, float oy);

  void translate_left(float ox, float oy);

  void rotate(float degrees, float center_x, float center_y);

  void rotate_left(float degrees, float center_x, float center_y);

  /* Push the current transform settings onto a stack. pop() can later be used
   * to return these settings. Note that this only affects the transform
   * settings of this canvas. */
  void push();

  void pop();

  /* Reset the transform to the identity transform and clear the transform
   * stack. */
  void reset();

  /* Control whether or not the coordinates should be auto-scaled to simulate a
   * 1600-900 sized screen. Only changes behavior for hud canvases. By default
   * this is set to true.
   */
  bool scale_hud();

  void scale_hud(bool scale_hud);

}

/* Definitions match those described in
 * http://www.cplusplus.com/reference/ctime/tm/
 */
class timedate {
  int sec();

  int min();

  int hour();

  int mday();

  int mon();

  int year();

  int wday();

  int yday();

  int isdst();

}

class fog_setting {
  /* Access the layer and sublayer fog colours. */
  uint colour(uint layer, uint sublayer);

  void colour(uint layer, uint sublayer, uint colour);

  void layer_colour(uint layer, uint colour);

  /* Access the layer and sublayer fog percents. The percent for each
   * layer/sublayer indicates how to mix the fog colour with the graphics
   * on that layer. A percent of 0 ignores the fog colour and a percent
   * of 1 replaces all colours in drawn graphics with the fog colour. */
  float percent(uint layer, uint sublayer);

  void percent(uint layer, uint sublayer, float percent);

  void layer_percent(uint layer, float percent);

  /* Access where on the screen the screen the middle background colour
   * is rendered. Should be between 0 and 1 with 0 indicating the top of
   * the screen and 1 the bottom. */
  float bg_mid_point();

  void bg_mid_point(float bg_mid_point);

  /* Access the background colour at the top of the screen. */
  uint bg_top();

  void bg_top(uint bg_top);

  /* Access the background colour in the middle of the screen. */
  uint bg_mid();

  void bg_mid(uint bg_mid);

  /* Access the background colour at the bottom of the screen. */
  uint bg_bot();

  void bg_bot(uint bg_bot);

  /* Access the star saturation at the top of the screen. Should be
   * a number between 0 and 1. */
  float stars_top();

  void stars_top(float s_top);

  /* Access the star saturation in the middle of the screen. */
  float stars_mid();

  void stars_mid(float s_mid);

  /* Access the star saturation at the bottom of the screen. */
  float stars_bot();

  void stars_bot(float s_bot);

}

class editor_api {
  /* Deprecated. Use :ref:`class input_api` instead. */
  bool key_check_vk(int vk);

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool key_check_pressed_vk(int vk);

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool key_check_gvb(int gvb);

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool key_check_pressed_gvb(int gvb);

  /* Deprecated. Use :ref:`class input_api` instead. */
  void key_clear_gvb(int gvb);

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool key_check_vb(int player, int vb);

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool key_check_pressed_vb(int player, int vb);

  /* Deprecated. Use :ref:`class input_api` instead. */
  void key_clear_vb(int player, int vb);

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool has_focus();

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool is_polling_keyboard();

  /* Deprecated. Use :ref:`class input_api` instead. */
  bool poll_keyboard();

  /* Return the currently active editor tab. */
  string editor_tab();

  /* Returns true if the change succeeded. */
  bool editor_tab(string tab_name);

  /* Returns the selected layer. */
  int get_selected_layer();

  /* Sets the selected layer. */
  void set_selected_layer(int layer);

  /* Returns the selected sub layer for props. */
  int get_selected_sub_layer();

  /* Sets the selected sub layer for props. */
  void set_selected_sub_layer(int sub_layer);

  /* Returns true if the given layer is visible. */
  bool get_layer_visible(int layer);

  /* Sets the visibility of the given layer. */
  void set_layer_visible(int layer, bool visible);

  /* Returns true if the given layer is locked. */
  bool get_layer_locked(int layer);

  /* Sets the locked state of the given layer. */
  void set_layer_locked(int layer, bool visible);

  /* Returns true if the given layer is visible and not locked. */
  bool check_layer_filter(int layer);

  /* Returns the selected trigger. */
  entity@ get_selected_trigger();

  /* Returns the selected entity. */
  entity@ get_selected_entity();

  /* Access whether the help menu is visible. */
  bool help_screen_vis();

  void help_screen_vis(bool vis);

  bool hide_gui();

  void hide_gui(bool hide);

  bool hide_panels_gui();

  void hide_panels_gui(bool hide);

  bool hide_toolbar_gui();

  void hide_toolbar_gui(bool hide);

  bool hide_layers_gui();

  void hide_layers_gui(bool hide);

  /* Returns true if the mouse is within the editor GUI menus */
  bool mouse_in_gui();

  /* Force the editor to act as if the mouse is inside of the GUI menu for the
   * remainder of the frame */
  void force_mouse_in_gui();

  /* Returns the width of the left menu panel. */
  int menu_left_panel_width();

  /* Returns the width of the right menu panel. */
  int menu_right_panel_width();

  rectangle@ select_rectangle();

  /* Return the number of selected entities. */
  uint selected_entity_count();

  /* Return the index-th selected entity or null if no entity exists at that index. */
  entity@ selected_entity(uint index);

  /* Return the number of selected props. */
  uint selected_prop_count();

  /* Return the index-th selected prop or null if no prop exists at that index. */
  prop@ selected_prop(uint index);

}

class input_api {
  /* Returns text input recorded over the last frame. */
  string get_text();

  void set_text(string text);

  /* Returns the clipboard text. */
  string get_clipboard();

  /* Returns false on failure?? */
  bool set_clipboard(string text);

  /* Returns the x coordinate of the mouse in the hud coordinate space. If scale
   * is set to true will auto scale the coordinates to simulate a 1600-900
   * screen size. Will range between -width/2 and width/2.
   */
  float mouse_x_hud(bool scale);

  /* Equivalent to mouse_x_hud(false) */
  float mouse_x_hud();

  /* Returns the y coordinate of the mouse in the hud coordinate space. If scale
   * is set to true will auto scale the coordinates to simulate a 1600-900
   * screen size. Will range between -height/2 and height/2.
   */
  float mouse_y_hud(bool scale);

  /* Equivalent to mouse_y_hud(false) */
  float mouse_y_hud();

  /* Returns the x coordinate of the mouse for the given layer. */
  float mouse_x_world(int layer);

  /* Returns the y coordinate of the mouse for the given layer. */
  float mouse_y_world(int layer);

  /* Return the current screen width in pixels. */
  float screen_width();

  /* Return the current screen height in pixels. */
  float screen_height();

  /* Returns the mouse state as a bitmask. The
   * bits correspond to the following button states:
   *
   * Bitmask:
   *   :0x1: wheel up
   *   :0x2: wheel down
   *   :0x4: left down
   *   :0x8: right down
   *   :0x10: middle down
   *   :0x20: left pressed
   *   :0x40: right pressed
   *   :0x80: middle pressed
   *   :0x100: left release
   *   :0x200: right release
   *   :0x400: middle release
   */
  int mouse_state();

  /* Returns true if the key is currently pressed. vk should be a
   * virtual key keycode. See
   * https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes.
   * This is raw access to the underlying key state and will never work
   * in net games. Button states should only be accessed in step() functions.
   */
  bool key_check_vk(int vk);

  /* Returns true if the key was pressed this frame. */
  bool key_check_pressed_vk(int vk);

  /* Returns true if the key was released this frame. */
  bool key_check_released_vk(int vk);

  /* Returns true if the "global virtual button" is currently pressed.
   * Refer to the GLOBAL_VIRTUAL_BUTTON enum for options for this value.
   * These button states are fully managed in net games.
   */
  bool key_check_gvb(int gvb);

  /* Returns true if the global virtual button is pressed this frame. */
  bool key_check_pressed_gvb(int gvb);

  /* Returns true if the global virtual button is released this frame. */
  bool key_check_released_gvb(int gvb);

  /* Unset the global virtual button state. */
  void key_clear_gvb(int gvb);

  /* Returns true if the "player virtual button" is currently pressed.
   * Refer to the PLAYER_VIRTUAL_BUTTON enum for options for this value.
   * These button states are fully managed in net games.
   */
  bool key_check_vb(int player, int vb);

  /* Returns true if the global virtual button is pressed this frame. */
  bool key_check_pressed_vb(int player, int vb);

  /* Unset the global virtual button state. */
  void key_clear_vb(int player, int vb);

  /* Returns true if a UI control has focus */
  bool has_focus();

  /* Returns true if a UI control,
   * e.g. a text box, is taking keyboard input
   */
  bool is_polling_keyboard();

  /* Polls the keyboard for one frame, blocking shortcuts such as
   * frame advance
   */
  bool poll_keyboard();

}

/* API methods useful within a nexus. These allow you to access/modify
 * information about used keys and what levels have been completed within
 * the nexus.
 *
 * Internally, Dustforce tracks two types of information within a nexus.
 *
 * - Key usage information. This information is updated whenever you open
 *   a new door.
 * - Level completion information. This includes information about highest
 *   ranking score/completion, best time, and what key type the level
 *   granted.
 *
 * Notably keys earned and keys available are not tracked directly and are
 * just a function of the above two pieces of information. The game also keeps
 * track of what doors have been opened by inserting level score data when
 * the level is first opened that has completion and finesse each set to -1.
 */
class nexus_api {
  /* Get the number of keys that have been used of each time. This reflects
   * directly what is persisted to disk. ngplus controls whether all doors
   * are automatically in an open state.
   */
  void get_keys_used(int &out wood, int &out silver, int &out gold, int &out red, bool &out ngplus);

  /* Update key usage and save to disk. */
  void set_keys_used(int wood, int silver, int gold, int red, bool ngplus);

  /* Convenience function to calculate number of keys that have been earned
   * of each type. Subtract out the used key counts to get the number of
   * available keys. */
  void get_keys_earned(int &out wood, int &out silver, int &out gold, int &out red);

  /* Get count of levels that have score data in this nexus root */
  uint score_count();

  /* Get the name of the i-th level in this nexus root */
  string score_level(uint index);

  /* Lookup the score data for a given level.
   *
   * Arguments:
   *   :level: The name of the level to lookup
   *   :thorough: The "completion" score 1-5 mapping to D-S. -1 indicates the
   *              level door has been opened but has not be completed.
   *   :finesse: The "finesse" score with the same semantics as `thorough`.
   *   :time: The fastest completion time for the level. Dustforce tracks this
   *          although does nothing with this information natively.
   *   :key_type: The type of key rewarded for beating the level. 0=none,
   *              1=silver, 2=gold, 3=red, 4=wood.
   *
   * Returns:
   *   True if an entry was found for the level and the out variables have been
   *   set.
   */
  bool score_lookup(string level, int &out thorough, int &out finesse, float &out time, int &out key_type);

  /* Set the score data for a given level. Variables have the same semantics as
   * in `nexus_api.score_lookup`. */
  void score_set(string level, int thorough, int finesse, float time, int key_type);

}

