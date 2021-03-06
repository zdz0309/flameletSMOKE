{
//- Modifyed for flamelet

//- Equation from libOpenSmoke

rho = thermo.rho();
rho = max(rho, rhoMin);
rho = min(rho, rhoMax);

volScalarField AU = UEqn().A();
U = UEqn().H()/AU;
UEqn.clear();
phi = fvc::interpolate(rho*U) & mesh.Sf();
bool closedVolume = adjustPhi(phi, U, p);

for (int nonOrth=0; nonOrth<=simple.nNonOrthCorr(); nonOrth++)
{
	fvScalarMatrix pEqn
	(
	     fvm::laplacian(rho/AU, p) == fvc::div(phi)
	);

	pEqn.setReference(pRefCell, pRefValue);

	pEqn.solve();

	if (nonOrth == simple.nNonOrthCorr())
	{
	    phi -= pEqn.flux();
	}
}

#include "incompressible/continuityErrs.H"

// Explicitly relax pressure for momentum corrector
p.relax();

U -= fvc::grad(p)/AU;
U.correctBoundaryConditions();

bound(p, pMin);

// For closed-volume cases adjust the pressure and density levels
// to obey overall mass continuity
if (closedVolume)
{
	p += (initialMass - fvc::domainIntegrate(thermo.psi()*p))/fvc::domainIntegrate(thermo.psi());
}

rho = thermo.rho();
rho.relax();

if(monitoring != true)
{
Info<< "rho max/min : " << max(rho).value() << " " << min(rho).value() << endl;
}
}
