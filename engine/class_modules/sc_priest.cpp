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

  void init_base_stats() override;
  void init_spells() override;

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

void priest_t::init_base_stats()
{
  player_t::init_base_stats();

  // original classic crit rates according to
  // * https://github.com/wowsims/classic/blob/master/sim/core/base_stats.go#L106
  // TODO: Verify in-game
  base.attack_crit_chance = get_passive_player_value( 0.03, "all_crit" );
  base.spell_crit_chance = get_passive_player_value( 0.008, "all_crit" );
}

void priest_t::init_spells()
{
  player_t::init_spells();

  parse_all_class_passives();
  parse_all_passive_talents();
}

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

// Shadow Word: Pain

constexpr std::array shadow_word_pain_ranks {
  //            spell
  spell_rank_t{   589 },
  spell_rank_t{   594 },
  spell_rank_t{   970 },
  spell_rank_t{   992 },
  spell_rank_t{  2767 },
  spell_rank_t{ 10892 },
  spell_rank_t{ 10893 },
  spell_rank_t{ 10894 },
};
struct shadow_word_pain_t : public priest_spell_t
{
  shadow_word_pain_t( priest_t* player, util::string_view options_str ) :
    priest_spell_t( "shadow_word_pain", player, player->find_spell( player->find_rank( shadow_word_pain_ranks ).spell_id ) )
  {
    parse_options( options_str );
  }
};

// Mind Blast

constexpr std::array mind_blast_ranks {
  //            spell
  spell_rank_t{  8092 },
  spell_rank_t{  8102 },
  spell_rank_t{  8103 },
  spell_rank_t{  8104 },
  spell_rank_t{  8105 },
  spell_rank_t{  8106 },
  spell_rank_t{ 10945 },
  spell_rank_t{ 10946 },
  spell_rank_t{ 10947 },
};
struct mind_blast_t : public priest_spell_t
{
  mind_blast_t( priest_t* player, util::string_view options_str ) :
    priest_spell_t( "mind_blast", player, player->find_spell( player->find_rank( mind_blast_ranks ).spell_id ) )
  {
    parse_options( options_str );
  }
};

// Devouring Plague

// TODO: Heal the caster for the damage dealt
constexpr std::array devouring_plague_ranks {
  //            spell
  spell_rank_t{  2944 },
  spell_rank_t{ 19276 },
  spell_rank_t{ 19277 },
  spell_rank_t{ 19278 },
  spell_rank_t{ 19279 },
  spell_rank_t{ 19280 },
};
struct devouring_plague_t : public priest_spell_t
{
  devouring_plague_t( priest_t* player, util::string_view options_str ) :
    priest_spell_t( "devouring_plague", player, player->find_spell( player->find_rank( devouring_plague_ranks ).spell_id ) )
  {
    parse_options( options_str );
  }
};

action_t* priest_t::create_action( util::string_view name, util::string_view options_str )
{
  if ( name == "smite" )
    return new smite_t( this, options_str );
  if ( name == "shadow_word_pain" )
    return new shadow_word_pain_t( this, options_str );
  if ( name == "mind_blast" )
    return new mind_blast_t( this, options_str );
  if ( name == "devouring_plague" )
    return new devouring_plague_t( this, options_str );

  return player_t::create_action( name, options_str );
}

}  // namespace priestspace

const module_t* module_t::priest()
{
  static priestspace::priest_module_t m;
  return &m;
}