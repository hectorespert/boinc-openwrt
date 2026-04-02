#include "Enums.h"
#include <iostream>
#include "globals.h"
#include <SIMDHelpers.h>
#include "CalcStrategyNone.hpp"
#include "declarations.h"


std::string GetCpuInfo()
{
	return "";
}


void GetSupportedSIMDs()
{
	// No SIMD support in this implementation
}

SIMDEnum CheckSupportedSIMDs(SIMDEnum simd)
{
	SIMDEnum tempSimd = simd;
	if (simd == SIMDEnum::OptASIMD)
	{
		simd = CPUopt.hasASIMD
				   ? SIMDEnum::OptASIMD
				   : SIMDEnum::OptNONE;
	}

	if (tempSimd != simd)
	{
		std::cerr << "Choosen optimization " << getSIMDEnumName(tempSimd) << " is not supported. Switching to " << getSIMDEnumName(simd) << "." << std::endl;
	}

	return simd;
}

SIMDEnum GetBestSupportedSIMD()
{

	std::cerr << "Not using SIMD optimizations." << std::endl;
	return SIMDEnum::OptNONE;
}

void SetOptimizationStrategy(SIMDEnum useOptimization)
{
	switch (useOptimization)
	{
		case SIMDEnum::OptNONE:
		default:
			calcCtx.SetStrategy(CreateAlignedShared<CalcStrategyNone>(64));
			break;
	}
}
