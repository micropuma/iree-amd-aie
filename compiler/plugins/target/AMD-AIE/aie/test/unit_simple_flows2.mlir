
// RUN: iree-opt --amdaie-create-pathfinder-flows %s | FileCheck %s

// CHECK-LABEL:   aie.device(xcvc1902) {
// CHECK:           %[[TILE_2_3:.*]] = aie.tile(2, 3)
// CHECK:           %[[TILE_2_2:.*]] = aie.tile(2, 2)
// CHECK:           %[[TILE_1_1:.*]] = aie.tile(1, 1)
// CHECK:           %[[SWITCHBOX_2_2:.*]] = aie.switchbox(%[[TILE_2_2]]) {
// CHECK-DAG:         aie.connect<NORTH : 0, CORE : 1>
// CHECK-DAG:         aie.connect<CORE : 0, SOUTH : 0>
// CHECK:           }
// CHECK:           %[[SWITCHBOX_2_3:.*]] = aie.switchbox(%[[TILE_2_3]]) {
// CHECK-DAG:         aie.connect<CORE : 0, SOUTH : 0>
// CHECK:           }
// CHECK:           %[[SWITCHBOX_1_1:.*]] = aie.switchbox(%[[TILE_1_1]]) {
// CHECK-DAG:         aie.connect<EAST : 0, CORE : 0>
// CHECK:           }
// CHECK:           %[[TILE_2_1:.*]] = aie.tile(2, 1)
// CHECK:           %[[SWITCHBOX_2_1:.*]] = aie.switchbox(%[[TILE_2_1]]) {
// CHECK-DAG:         aie.connect<NORTH : 0, WEST : 0>
// CHECK:           }
// CHECK:         }

module {
  aie.device(xcvc1902) {
    %t23 = aie.tile(2, 3)
    %t22 = aie.tile(2, 2)
    %t11 = aie.tile(1, 1)
    aie.flow(%t23, CORE : 0, %t22, CORE : 1)
    aie.flow(%t22, CORE : 0, %t11, CORE : 0)
  }
}
