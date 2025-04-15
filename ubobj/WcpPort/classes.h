// Build a dictionary
// brooke.russell@yale.edu

#include "canvas/Persistency/Common/Wrapper.h"
#include "ubobj/WcpPort/NuSelectionCharge.h"
#include "ubobj/WcpPort/NuSelectionContainment.h"
#include "ubobj/WcpPort/NuSelectionMatch.h"
#include "ubobj/WcpPort/NuSelectionTruth.h"

// for STM labels
#include "ubobj/WcpPort/NuSelectionSTM.h"

// for BDT input variables
#include "ubobj/WcpPort/NuSelectionBDT.h"

// for kinetic variables
#include "ubobj/WcpPort/NuSelectionKINE.h"

// Space points.
#include "ubobj/WcpPort/SpacePointStructs.h"

#include <vector>

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

// Change the template declaration
template class art::Wrapper<std::vector<TrecSpacePoint>>;
template class art::Wrapper<std::vector<TrecchargeSpacePoint>>;
template class art::Wrapper<std::vector<TrecchargeblobSpacePoint>>;
template class art::Wrapper<std::vector<TclusterSpacePoint>>;
