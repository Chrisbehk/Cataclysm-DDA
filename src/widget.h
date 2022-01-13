#pragma once
#ifndef CATA_SRC_WIDGET_H
#define CATA_SRC_WIDGET_H

#include <string>
#include <vector>

#include "avatar.h"
//#include "cata_variant.h"
#include "enum_traits.h"
#include "generic_factory.h"
#include "panels.h"
#include "string_id.h"
#include "translations.h"
#include "type_id.h"

// These are the supported data variables for widgets, defined as enum widget_var.
// widget_var names may be given as the "var" field in widget JSON.
enum class widget_var : int {
    focus,          // Current focus, integer
    move,           // Current move counter, integer
    move_cost,      // Modified base movement cost, integer (from run_cost)
    pain,           // Current perceived pain, integer
    sound,          // Current sound level, integer
    speed,          // Current speed, integer
    stamina,        // Current stamina 0-10000, greater being fuller stamina reserves
    fatigue,        // Current fatigue, integer
    health,         // Current hidden health value, -200 to +200
    mana,           // Current available mana, integer
    max_mana,       // Current maximum mana, integer
    morale_level,   // Current morale level, integer (may be negative)
    weariness_level, // Current weariness level, integer
    stat_str,       // Base STR (strength) stat, integer
    stat_dex,       // Base DEX (dexterity) stat, integer
    stat_int,       // Base INT (intelligence) stat, integer
    stat_per,       // Base PER (perception) stat, integer
    bp_hp,          // Current hit points of given "bodypart", integer
    bp_encumb,      // Current encumbrance of given "bodypart", integer
    bp_warmth,      // Current warmth of given "bodypart", integer
    bp_wetness,     // Current wetness of given "bodypart", integer
    hunger,         // TODO
    thirst,         // TODO
    mood,           // TODO
    cardio_fit,     // Cardio fitness, integer near BMR
    cardio_acc,     // Cardio accumulator, integer
    // Text vars
    activity_text,  // Activity level text, color string
    body_temp_text, // Felt body temperature, color string
    bp_status_text, // Status of bodypart (bleeding, bitten, and/or infected)
    compass_text,   // Compass / visible threats by cardinal direction
    compass_legend_text, // Names of visible creatures that appear on the compass
    date_text,      // Current date, in terms of day within season
    env_temp_text,  // Environment temperature, if character has thermometer
    fatigue_text,   // Fagitue description text, color string
    health_text,    // Hidden health message, color string
    hunger_text,    // Hunger description text, color string
    lighting_text,  // Current light level, color string
    mood_text,      // Mood as a text emote, color string
    moon_phase_text,// Current phase of the moon
    move_mode_letter, // Movement mode, color letter (W/R/C/P)
    move_mode_text, // Movement mode, color text (walking/running/crouching/prone)
    pain_text,      // Pain description text, color string
    place_text,     // Place name in world where character is
    power_text,     // Remaining power from bionics, color string
    rad_badge_text, // Color indicator for radiation badge
    safe_mode_text, // Safe mode text, color string
    style_text,     // Active martial arts style name
    thirst_text,    // Thirst description text, color string
    time_text,      // Current time - exact if character has a watch, approximate otherwise
    veh_azimuth_text, // Azimuth or heading in degrees, string
    veh_cruise_text, // Current/target cruising speed in vehicle, color string
    veh_fuel_text,  // Current/total fuel for active vehicle engine, color string
    weariness_text, // Weariness description text, color string
    weary_malus_text, // Weariness malus or penalty
    weather_text,   // Weather/sky conditions (if visible), color string
    weight_text,    // Weight description text, color string
    wielding_text,  // Currently wielded weapon or item name
    wind_text,      // Wind level and direction, color string
    last // END OF ENUMS
};

// Use enum_traits for generic iteration over widget_var, and string (de-)serialization.
// Use io::string_to_enum<widget_var>( widget_string ) to convert a string to widget_var.
template<>
struct enum_traits<widget_var> {
    static constexpr widget_var last = widget_var::last;
};

// This is deliberately separate from "direction".
// The values correspond to the indexed directions returned from avatar::get_mon_visible
enum class cardinal_direction : int {
    NORTH = 0,
    NORTHEAST = 1,
    EAST = 2,
    SOUTHEAST = 3,
    SOUTH = 4,
    SOUTHWEST = 5,
    WEST = 6,
    NORTHWEST = 7,
    LOCAL = 8,
    num_cardinal_directions
};

// Use enum_traits for generic iteration over cardinal_direction, and string (de-)serialization.
// Use io::string_to_enum<cardinal_direction>( string ) to convert a string to cardinal_direction.
template<>
struct enum_traits<cardinal_direction> {
    static constexpr cardinal_direction last = cardinal_direction::num_cardinal_directions;
};

// Use generic_factory for loading JSON data.
class JsonObject;
template<typename T>
class generic_factory;

// Forward declaration, due to codependency on panels.h
class window_panel;

// A widget is a UI element displaying information from the underlying value of a widget_var.
// It may be loaded from a JSON object having "type": "widget".
class widget
{
    private:
        friend class generic_factory<widget>;

        widget_id id;
        bool was_loaded = false;

    public:
        widget() = default;
        explicit widget( const widget_id &id ) : id( id ) {}

        // Attributes from JSON
        // ----
        // Display style to indicate the value: "numeric", "graph", "text"
        std::string _style;
        // Displayed label in the UI
        translation _label;
        // Binding variable enum like stamina, bp_hp or stat_dex
        widget_var _var;
        // Minimum var value, optional
        int _var_min = 0;
        // Maximum var value, required for graph widgets
        int _var_max = 10;
        // Body part variable is linked to
        bodypart_id _bp_id;
        // Width in characters of widget, not including label
        int _width = 0;
        // Height in characters of widget, only matters for style == widget
        int _height = 0;
        // String of symbols for graph widgets, mapped in increasing order like "0123..."
        std::string _symbols;
        // Graph fill style ("bucket" or "pool")
        std::string _fill;
        // String values mapped to numeric values or ranges
        std::vector<translation> _strings;
        // Colors mapped to values or ranges
        std::vector<nc_color> _colors;
        // Child widget ids for layout style
        std::vector<widget_id> _widgets;
        // Child widget layout arrangement / direction
        std::string _arrange;
        // Compass direction corresponding to the indexed directions from avatar::get_mon_visible
        cardinal_direction _direction;
        // Flags for special widget behaviors
        std::set<flag_id> _flags;

        // Load JSON data for a widget (uses generic factory widget_factory)
        static void load_widget( const JsonObject &jo, const std::string &src );
        void load( const JsonObject &jo, const std::string &src );
        // Finalize anything that must wait until all widgets are loaded
        static void finalize();
        // Reset to defaults using generic widget_factory
        static void reset();
        // Get all widget instances from the factory
        static const std::vector<widget> &get_all();

        // Layout this widget within max_width, including child widgets. Calling layout on a regular
        // (non-layout style) widget is the same as show(), but will pad with spaces inside the
        // label area, so the returned string is equal to max_width.
        std::string layout( const avatar &ava, unsigned int max_width = 0 );
        // Display labeled widget, with value (number, graph, or string) from an avatar
        std::string show( const avatar &ava, unsigned int max_width );
        // Return a window_panel for rendering this widget at given width (and possibly height)
        window_panel get_window_panel( const int width, const int req_height = 1 );
        // Return a colorized string for a _var associated with a description function
        std::string color_text_function_string( const avatar &ava, unsigned int max_width );
        // Return true if the current _var is one which uses a description function
        bool uses_text_function();

        // Evaluate and return the bound "var" associated value for an avatar
        int get_var_value( const avatar &ava ) const;
        // Return the maximum "var" value from "var_max", or max for avatar (HP, mana, etc.)
        int get_var_max( const avatar &ava ) const;
        // True if this widget has the given flag. Used to specify certain behaviors.
        bool has_flag( const flag_id &flag ) const;
        bool has_flag( const std::string &flag ) const;

        // Return a color-enhanced value_string
        std::string color_value_string( int value, int value_max = 0 );
        // Return a string for how a given value will render in the UI
        std::string value_string( int value, int value_max = 0 );
        // Return a suitable color for a given value
        nc_color value_color( int value, int value_max = 0 );

        // Return a formatted numeric string
        std::string number( int value, int value_max = 0 );
        // Return the text phrase mapped to a given value for "text" style
        std::string text( int value, int value_max = 0 );
        // Return the graph part of this widget, rendered with "bucket" or "pool" fill
        std::string graph( int value, int value_max = 0 );
        // Takes a string generated by widget::layout and draws the text to the window w.
        // If the string contains newline characters, the text is broken up into lines.
        // Returns the new row index after drawing.
        // Note: Not intended to be called directly, only public for unit testing.
        static int custom_draw_multiline( const std::string &widget_string, const catacurses::window &w,
                                          const int margin, const int width, int row_num );
};

#endif // CATA_SRC_WIDGET_H

