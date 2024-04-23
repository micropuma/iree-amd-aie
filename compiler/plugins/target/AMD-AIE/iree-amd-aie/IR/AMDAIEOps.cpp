// Copyright 2024 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree-amd-aie/IR/AMDAIEOps.h"
#include "iree-amd-aie/IR/AMDAIEDialect.h"
#include "llvm/ADT/TypeSwitch.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Transform/IR/TransformOps.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpDefinition.h"

#define GET_OP_CLASSES
#include "iree-amd-aie/IR/AMDAIEOps.cpp.inc"

namespace mlir::iree_compiler::AMDAIE {

void AMDAIEDialect::initializeAMDAIEOps() {
  addOperations<
#define GET_OP_LIST
#include "iree-amd-aie/IR/AMDAIEOps.cpp.inc"
      >();
}

//===----------------------------------------------------------------------===//
// AMDAIE_ControlCodeOp
//===----------------------------------------------------------------------===//

LogicalResult ControlCodeOp::verify() {
  // Verify that this ControlCodeOp contains a EndOp terminator if one
  // exists.
  if (failed(OpTrait::SingleBlockImplicitTerminator<EndOp>::Impl<
             ControlCodeOp>::verifyRegionTrait(*this))) {
    return failure();
  }
  return success();
}

//===----------------------------------------------------------------------===//
// AMDAIE_CoreOp
//===----------------------------------------------------------------------===//

LogicalResult CoreOp::verify() {
  // Verify that this CoreOp contains a EndOp terminator if one
  // exists.
  if (failed(OpTrait::SingleBlockImplicitTerminator<EndOp>::Impl<
             CoreOp>::verifyRegionTrait(*this))) {
    return failure();
  }
  return success();
}

//===----------------------------------------------------------------------===//
// AMDAIE_DmaCpyNdOp
//===----------------------------------------------------------------------===//

// Build a DmaCpyNdOp with mixed static and dynamic entries.
void DmaCpyNdOp::build(OpBuilder &b, OperationState &result, Value target,
                       ArrayRef<OpFoldResult> targetOffsets,
                       ArrayRef<OpFoldResult> targetSizes,
                       ArrayRef<OpFoldResult> targetStrides, Value source,
                       ArrayRef<OpFoldResult> sourceOffsets,
                       ArrayRef<OpFoldResult> sourceSizes,
                       ArrayRef<OpFoldResult> sourceStrides) {
  SmallVector<int64_t> staticTargetOffsets, staticTargetSizes,
      staticTargetStrides;
  SmallVector<int64_t> staticSourceOffsets, staticSourceSizes,
      staticSourceStrides;
  SmallVector<Value> dynamicTargetOffsets, dynamicTargetSizes,
      dynamicTargetStrides;
  SmallVector<Value> dynamicSourceOffsets, dynamicSourceSizes,
      dynamicSourceStrides;
  dispatchIndexOpFoldResults(targetOffsets, dynamicTargetOffsets,
                             staticTargetOffsets);
  dispatchIndexOpFoldResults(targetSizes, dynamicTargetSizes,
                             staticTargetSizes);
  dispatchIndexOpFoldResults(targetStrides, dynamicTargetStrides,
                             staticTargetStrides);
  dispatchIndexOpFoldResults(sourceOffsets, dynamicSourceOffsets,
                             staticSourceOffsets);
  dispatchIndexOpFoldResults(sourceSizes, dynamicSourceSizes,
                             staticSourceSizes);
  dispatchIndexOpFoldResults(sourceStrides, dynamicSourceStrides,
                             staticSourceStrides);
  build(b, result, b.getIndexType(), target, dynamicTargetOffsets,
        dynamicTargetSizes, dynamicTargetStrides, staticTargetOffsets,
        staticTargetSizes, staticTargetStrides, source, dynamicSourceOffsets,
        dynamicSourceSizes, dynamicSourceStrides, staticSourceOffsets,
        staticSourceSizes, staticSourceStrides);
}

// Build a DmaCpyNdOp with static entries.
void DmaCpyNdOp::build(OpBuilder &b, OperationState &result, Value target,
                       ArrayRef<int64_t> targetOffsets,
                       ArrayRef<int64_t> targetSizes,
                       ArrayRef<int64_t> targetStrides, Value source,
                       ArrayRef<int64_t> sourceOffsets,
                       ArrayRef<int64_t> sourceSizes,
                       ArrayRef<int64_t> sourceStrides) {
  SmallVector<OpFoldResult> targetOffsetValues = llvm::to_vector<4>(
      llvm::map_range(targetOffsets, [&](int64_t v) -> OpFoldResult {
        return b.getI64IntegerAttr(v);
      }));
  SmallVector<OpFoldResult> targetSizeValues = llvm::to_vector<4>(
      llvm::map_range(targetSizes, [&](int64_t v) -> OpFoldResult {
        return b.getI64IntegerAttr(v);
      }));
  SmallVector<OpFoldResult> targetStrideValues = llvm::to_vector<4>(
      llvm::map_range(targetStrides, [&](int64_t v) -> OpFoldResult {
        return b.getI64IntegerAttr(v);
      }));
  SmallVector<OpFoldResult> sourceOffsetValues = llvm::to_vector<4>(
      llvm::map_range(sourceOffsets, [&](int64_t v) -> OpFoldResult {
        return b.getI64IntegerAttr(v);
      }));
  SmallVector<OpFoldResult> sourceSizeValues = llvm::to_vector<4>(
      llvm::map_range(sourceSizes, [&](int64_t v) -> OpFoldResult {
        return b.getI64IntegerAttr(v);
      }));
  SmallVector<OpFoldResult> sourceStrideValues = llvm::to_vector<4>(
      llvm::map_range(sourceStrides, [&](int64_t v) -> OpFoldResult {
        return b.getI64IntegerAttr(v);
      }));
  build(b, result, target, targetOffsetValues, targetSizeValues,
        targetStrideValues, source, sourceOffsetValues, sourceSizeValues,
        sourceStrideValues);
}

// Build a DmaCpyNdOp with dynamic entries.
void DmaCpyNdOp::build(OpBuilder &b, OperationState &result, Value target,
                       ValueRange targetOffsets, ValueRange targetSizes,
                       ValueRange targetStrides, Value source,
                       ValueRange sourceOffsets, ValueRange sourceSizes,
                       ValueRange sourceStrides) {
  SmallVector<OpFoldResult> targetOffsetValues =
      llvm::to_vector<4>(llvm::map_range(
          targetOffsets, [](Value v) -> OpFoldResult { return v; }));
  SmallVector<OpFoldResult> targetSizeValues = llvm::to_vector<4>(
      llvm::map_range(targetSizes, [](Value v) -> OpFoldResult { return v; }));
  SmallVector<OpFoldResult> targetStrideValues =
      llvm::to_vector<4>(llvm::map_range(
          targetStrides, [](Value v) -> OpFoldResult { return v; }));
  SmallVector<OpFoldResult> sourceOffsetValues =
      llvm::to_vector<4>(llvm::map_range(
          sourceOffsets, [](Value v) -> OpFoldResult { return v; }));
  SmallVector<OpFoldResult> sourceSizeValues = llvm::to_vector<4>(
      llvm::map_range(sourceSizes, [](Value v) -> OpFoldResult { return v; }));
  SmallVector<OpFoldResult> sourceStrideValues =
      llvm::to_vector<4>(llvm::map_range(
          sourceStrides, [](Value v) -> OpFoldResult { return v; }));
  build(b, result, target, targetOffsetValues, targetSizeValues,
        targetStrideValues, source, sourceOffsetValues, sourceSizeValues,
        sourceStrideValues);
}

LogicalObjectFifoFromMemrefOp DmaCpyNdOp::getSourceObjectFifo() {
  return dyn_cast<LogicalObjectFifoFromMemrefOp>(getSource().getDefiningOp());
};

LogicalObjectFifoFromMemrefOp DmaCpyNdOp::getTargetObjectFifo() {
  return dyn_cast<LogicalObjectFifoFromMemrefOp>(getTarget().getDefiningOp());
};

//===----------------------------------------------------------------------===//
// AMDAIE_WorkgroupOp
//===----------------------------------------------------------------------===//

// Make sure the WorkgroupOp region is well-formed with a ControlCodeOp
// terminator
void WorkgroupOp::ensureTerminator(Region &region, OpBuilder &builder,
                                   Location loc) {
  OpTrait::SingleBlockImplicitTerminator<ControlCodeOp>::Impl<
      WorkgroupOp>::ensureTerminator(region, builder, loc);
  auto terminator =
      llvm::dyn_cast<ControlCodeOp>(region.front().getTerminator());
  if (terminator.getRegion().empty()) {
    Block *newBlock = builder.createBlock(&terminator.getRegion());
    builder.setInsertionPointToEnd(newBlock);
    builder.create<AMDAIE::EndOp>(builder.getUnknownLoc());
  }
}

// Builder that ensures the WorkgroupOp is well-formed with a block and a
// ControlCodeOp terminator
void WorkgroupOp::build(OpBuilder &builder, OperationState &result) {
  Region *bodyRegion = result.addRegion();
  OpBuilder::InsertionGuard guard(builder);
  builder.createBlock(bodyRegion);
  Block &bodyBlock = bodyRegion->front();
  builder.setInsertionPointToStart(&bodyBlock);
  WorkgroupOp::ensureTerminator(*bodyRegion, builder, result.location);
}

LogicalResult WorkgroupOp::verify() {
  // Verify that this WorkgroupOp contains a ControlCodeOp terminator if one
  // exists.
  if (failed(OpTrait::SingleBlockImplicitTerminator<ControlCodeOp>::Impl<
             WorkgroupOp>::verifyRegionTrait(*this))) {
    return failure();
  }
  return success();
}

}  // namespace mlir::iree_compiler::AMDAIE