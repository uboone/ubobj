//
// Build a dictionary.
//
// $Id: classes.h,v 1.8 2010/04/12 18:12:28  Exp $
// $Author:  $
// $Date: 2010/04/12 18:12:28 $
// 
// Original author Rob Kutschke, modified by wes
//

#include "canvas/Persistency/Common/Wrapper.h"
#include "canvas/Persistency/Common/Assns.h"
#include "UbooneOpticalFilter.h"
#include "SparseRawDigit.h"
#include "lardataobj/RecoBase/Wire.h"

//
// Only include objects that we would like to be able to put into the event.
// Do not include the objects they contain internally.
//

template class art::Wrapper< uboone::UbooneOpticalFilter >;
