// ==========================================================================
// Priest Sim File
// Contact: https://github.com/orgs/simulationcraft/teams/priest/members
// Wiki: https://github.com/simulationcraft/simc/wiki/Priests
// ==========================================================================

#include "simulationcraft.hpp"
#include "action/parse_effects.hpp"

namespace priestspace
{
struct priest_t final : public player_t
{
  priest_t( sim_t* sim, util::string_view name, race_e r = RACE_NONE ) :
    player_t( sim, PRIEST, name, r ) {}

  resource_e primary_resource() const override { return RESOURCE_MANA; }

  action_t* create_action( util::string_view name, util::string_view options_str ) override;
};

struct priest_module_t final : public module_t
{
  priest_module_t() : module_t( PRIEST ) {}

  player_t* create_player( sim_t* sim, util::string_view name, race_e r = RACE_NONE ) const override
  {
    auto p = new priest_t( sim, name, r );
    return p;
  }

  bool valid() const override
  {
    return true;
  }

  void register_hotfixes() const override {}

  void register_actor_initializers( sim_t* ) const override {}
};

template <class Base>
struct priest_action_t : public parse_action_effects_t<Base>
{
private:
  using ab = parse_action_effects_t<Base>;

public:
  using base_t = priest_action_t<Base>;

  template <typename... Args>
  priest_action_t( Args&&... args ) : ab( std::forward<Args>( args )... ) {}

  priest_t* p()
  { return debug_cast<priest_t*>( this->player ); }

  const priest_t* p() const
  { return debug_cast<const priest_t*>( this->player ); }
};

using priest_spell_t = priest_action_t<spell_t>;

// Smite

constexpr std::array smite_ranks {
  //            spell
  spell_rank_t{   585 },
  spell_rank_t{   591 },
  spell_rank_t{   598 },
  spell_rank_t{   984 },
  spell_rank_t{  1004 },
  spell_rank_t{  6060 },
  spell_rank_t{ 10933 },
  spell_rank_t{ 10934 },
};
struct smite_t : public priest_spell_t
{
  smite_t( priest_t* player, util::string_view options_str ) :
    priest_spell_t( "smite", player, player->find_spell( player->find_rank( smite_ranks ).spell_id ) )
  {
    parse_options( options_str );
  }
};

action_t* priest_t::create_action( util::string_view name, util::string_view options_str )
{
  if ( name == "smite" )
    return new smite_t( this, options_str );

  return player_t::create_action( name, options_str );
}

}  // namespace priestspace

const module_t* module_t::priest()
{
  static priestspace::priest_module_t m;
  return &m;
}