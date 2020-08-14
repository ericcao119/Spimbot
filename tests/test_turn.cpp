#include <catch2/catch.hpp>

#include "engine/turn.h"

TEST_CASE("Turn specifies turns correctly", "[turn]") {
    Turn turn;

    REQUIRE(turn.player_turn() == 0);

    SECTION("Test snake order before turn") {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            REQUIRE(turn.player_turn() == i);
            turn++;
        }
    }
    SECTION("Test snake order after turn") {
        while (turn.player_turn() != NUM_PLAYERS) {
            turn++;
        }

        for (int i = NUM_PLAYERS; i < 2 * NUM_PLAYERS; i++) {
            REQUIRE(turn.player_turn() == (2 * NUM_PLAYERS - i));
            turn++;
        }
        REQUIRE(turn.player_turn() == 0);  // test wrap around
    }

    // SECTION("Wrap around testing") {
    //     // Hidden to reduce assertion count 
    //     for (int j = 0; j < 131072; j++) {
    //         for (int i = 0; i < NUM_PLAYERS; i++) {
    //             REQUIRE(turn.player_turn() == i);
    //             turn++;
    //         }
    //         for (int i = NUM_PLAYERS; i < 2 * NUM_PLAYERS; i++) {
    //             REQUIRE(turn.player_turn() == (2 * NUM_PLAYERS - i));
    //             turn++;
    //         }
    //         REQUIRE(turn.player_turn() == 0);
    //     }
    // }
}