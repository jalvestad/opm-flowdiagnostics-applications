/*
  Copyright 2017 Statoil ASA.

  This file is part of the Open Porous Media Project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <opm/utility/ECLPvtCommon.hpp>

#include <opm/utility/ECLResultData.hpp>
#include <opm/utility/ECLUnitHandling.hpp>

#include <opm/parser/eclipse/Units/Units.hpp>

#include <functional>

#include <ert/ecl/ecl_kw_magic.h>

namespace {
    double fvfScale(const ::Opm::ECLUnits::UnitSystem& usys)
    {
        // B = [rVolume / sVolume(Liquid)]
        return usys.reservoirVolume()
            /  usys.surfaceVolumeLiquid();
    }

    double fvfGasScale(const ::Opm::ECLUnits::UnitSystem& usys)
    {
        // B = [rVolume / sVolume(Gas)]
        return usys.reservoirVolume()
            /  usys.surfaceVolumeGas();
    }

    double rsScale(const ::Opm::ECLUnits::UnitSystem& usys)
    {
        // Rs = [sVolume(Gas) / sVolume(Liquid)]
        return usys.surfaceVolumeGas()
            /  usys.surfaceVolumeLiquid();
    }

    double rvScale(const ::Opm::ECLUnits::UnitSystem& usys)
    {
        // Rv = [sVolume(Liq) / sVolume(Gas)]
        return usys.surfaceVolumeLiquid()
            /  usys.surfaceVolumeGas();
    }

    ::Opm::ECLPVT::ConvertUnits::Converter
    createConverterToSI(const double uscale)
    {
        return ::Opm::ECLPVT::ConvertUnits::Converter {
            [uscale](const double q) -> double
            {
                return ::Opm::unit::convert::from(q, uscale);
            }
        };
    }
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
density(const ::Opm::ECLUnits::UnitSystem& usys)
{
    return createConverterToSI(usys.density());
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
pressure(const ::Opm::ECLUnits::UnitSystem& usys)
{
    return createConverterToSI(usys.pressure());
}

::Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
compressibility(const ::Opm::ECLUnits::UnitSystem& usys)
{
    return createConverterToSI(1.0 / usys.pressure());
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
disGas(const ::Opm::ECLUnits::UnitSystem& usys)
{
    return createConverterToSI(rsScale(usys));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
vapOil(const ::Opm::ECLUnits::UnitSystem& usys)
{
    return createConverterToSI(rvScale(usys));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvf(const ::Opm::ECLUnits::UnitSystem& usys)
{
    return createConverterToSI(1.0 / fvfScale(usys));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfDerivPress(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/B)/dp
    const auto B_scale = fvfScale(usys);
    const auto P_scale = usys.pressure();

    return createConverterToSI(1.0 / (B_scale * P_scale));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfDerivVapOil(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/B)/dRv
    const auto B_scale  = fvfScale(usys);
    const auto Rv_scale = rvScale(usys);

    return createConverterToSI(1.0 / (B_scale * Rv_scale));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfVisc(const ::Opm::ECLUnits::UnitSystem& usys)
{
    const auto Bscale     = fvfScale(usys);
    const auto visc_scale = usys.viscosity();

    return createConverterToSI(1.0 / (Bscale * visc_scale));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfViscDerivPress(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/(B*mu))/dp
    const auto B_scale  = fvfScale(usys);
    const auto P_scale  = usys.pressure();
    const auto mu_scale = usys.viscosity();

    return createConverterToSI(1.0 / (B_scale * mu_scale * P_scale));
}

::Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfViscDerivVapOil(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/(B*mu))/dRv
    const auto B_scale  = fvfScale(usys);
    const auto mu_scale = usys.viscosity();
    const auto Rv_scale = rvScale(usys);

    return createConverterToSI(1.0 / (B_scale * mu_scale * Rv_scale));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfGas(const ::Opm::ECLUnits::UnitSystem& usys)
{
    return createConverterToSI(1.0 / fvfGasScale(usys));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfGasDerivPress(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/B)/dp
    const auto B_scale = fvfGasScale(usys);
    const auto P_scale = usys.pressure();

    return createConverterToSI(1.0 / (B_scale * P_scale));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfGasDerivVapOil(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/B)/dRv
    const auto B_scale  = fvfGasScale(usys);
    const auto Rv_scale = rvScale(usys);

    return createConverterToSI(1.0 / (B_scale * Rv_scale));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfGasVisc(const ::Opm::ECLUnits::UnitSystem& usys)
{
    const auto Bscale     = fvfGasScale(usys);
    const auto visc_scale = usys.viscosity();

    return createConverterToSI(1.0 / (Bscale * visc_scale));
}

Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfGasViscDerivPress(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/(B*mu))/dp
    const auto B_scale  = fvfGasScale(usys);
    const auto P_scale  = usys.pressure();
    const auto mu_scale = usys.viscosity();

    return createConverterToSI(1.0 / (B_scale * mu_scale * P_scale));
}

::Opm::ECLPVT::ConvertUnits::Converter
Opm::ECLPVT::CreateUnitConverter::ToSI::
recipFvfGasViscDerivVapOil(const ::Opm::ECLUnits::UnitSystem& usys)
{
    // d(1/(B*mu))/dRv
    const auto B_scale  = fvfGasScale(usys);
    const auto mu_scale = usys.viscosity();
    const auto Rv_scale = rvScale(usys);

    return createConverterToSI(1.0 / (B_scale * mu_scale * Rv_scale));
}

// =====================================================================

Opm::ECLPVT::PVDx::PVDx(ElemIt               xBegin,
                        ElemIt               xEnd,
                        const ConvertUnits&  convert,
                        std::vector<ElemIt>& colIt)
    : interp_(Extrap{}, xBegin, xEnd, colIt,
              convert.indep, convert.column)
{}

std::vector<double>
Opm::ECLPVT::PVDx::formationVolumeFactor(const std::vector<double>& p) const
{
    return this->computeQuantity(p,
        [this](const EvalPt& pt) -> double
    {
        // 1 / (1 / B)
        return 1.0 / this->fvf_recip(pt);
    });
}

std::vector<double>
Opm::ECLPVT::PVDx::viscosity(const std::vector<double>& p) const
{
    return this->computeQuantity(p,
        [this](const EvalPt& pt) -> double
    {
        // (1 / B) / (1 / (B * mu)
        return this->fvf_recip(pt) / this->fvf_mu_recip(pt);
    });
}

Opm::FlowDiagnostics::Graph
Opm::ECLPVT::PVDx::getPvtCurve(const RawCurve curve) const
{
    assert ((curve == RawCurve::FVF) ||
            (curve == RawCurve::Viscosity));

    const auto colID = (curve == RawCurve::FVF)
        ? std::size_t{0} : std::size_t{1};

    auto x = this->interp_.independentVariable();
    auto y = this->interp_.resultVariable(colID);

    assert ((x.size() == y.size()) && "Setup Error");

    // Post-process ordinates according to which curve is requested.
    if (curve == RawCurve::FVF) {
        // y == 1/B.  Convert to proper FVF.
        for (auto& yi : y) {
            yi = 1.0 / yi;
        }
    }
    else {
        // y == 1/(B*mu).  Extract viscosity term through the usual
        // conversion formula:
        //
        //    (1 / B) / (1 / (B*mu)).
        const auto b = this->interp_.resultVariable(0); // 1/B

        assert ((b.size() == y.size()) && "Setup Error");

        for (auto n = y.size(), i = 0*n; i < n; ++i) {
            y[i] = b[i] / y[i];
        }
    }

    // Graph == pair<vector<double>, vector<double>>
    return FlowDiagnostics::Graph { std::move(x), std::move(y) };
}

// =====================================================================

std::vector<double>
Opm::ECLPVT::surfaceMassDensity(const ECLInitFileData& init,
                                const ECLPhaseIndex    phase)
{
    const auto col = [phase]() -> std::size_t
    {
        // Column order: 0 <-> oil, 1 <-> water, 2 <-> gas

        switch (phase) {
        case ECLPhaseIndex::Aqua:   return std::size_t{ 1 };
        case ECLPhaseIndex::Liquid: return std::size_t{ 0 };
        case ECLPhaseIndex::Vapour: return std::size_t{ 2 };
        }

        throw std::invalid_argument {
            "Unsupported Phase ID"
        };
    }();

    const auto& tabdims = init.keywordData<int>("TABDIMS");
    const auto& tab     = init.keywordData<double>("TAB");

    // Subtract one to account for 1-based indices.
    const auto start = tabdims[ TABDIMS_IBDENS_OFFSET_ITEM ] - 1;
    const auto nreg  = tabdims[ TABDIMS_NTDENS_ITEM ];

    // Phase densities for 'phase' constitute 'nreg' consecutive entries
    // of TAB, starting at an appropriate column offset from the table's
    // 'start'.
    auto rho = std::vector<double> {
        &tab[ start + nreg*(col + 0) ],
        &tab[ start + nreg*(col + 1) ]
    };

    const auto& ih = init.keywordData<int>(INTEHEAD_KW);
    const auto u = ECLUnits::createUnitSystem(ih[ INTEHEAD_UNIT_INDEX ]);

    const auto dens_scale = u->density();

    for (auto& rho_i : rho) {
        rho_i = unit::convert::from(rho_i, dens_scale);
    }

    return rho;
}
