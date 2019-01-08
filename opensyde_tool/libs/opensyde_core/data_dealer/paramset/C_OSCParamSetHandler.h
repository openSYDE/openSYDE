//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for parameter set file operations (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCPARAMSETHANDLER_H
#define C_OSCPARAMSETHANDLER_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"
#include "C_OSCParamSetRawNode.h"
#include "C_OSCParamSetInterpretedNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///utility class providing access to parameter set files
class C_OSCParamSetHandler
{
public:
   //File
   stw_types::sint32 CreateCleanFileWithoutCRC(const stw_scl::C_SCLString & orc_FilePath,
                                               const bool oq_InterpretedDataOnly = false);
   stw_types::sint32 ReadFile(const stw_scl::C_SCLString & orc_FilePath, const bool oq_IgnoreCrc,
                              const bool oq_InterpretedDataOnly = false);
   stw_types::sint32 UpdateCRCForFile(const stw_scl::C_SCLString & orc_FilePath) const;

   //Data
   void ClearContent(void);
   stw_types::sint32 SetRawDataForNode(const C_OSCParamSetRawNode & orc_Content);
   stw_types::sint32 SetInterpretedDataForNode(const C_OSCParamSetInterpretedNode & orc_Content);
   const C_OSCParamSetRawNode * GetRawDataForNode(const stw_scl::C_SCLString & orc_NodeName) const;
   const C_OSCParamSetInterpretedNode * GetInterpretedDataForNode(const stw_scl::C_SCLString & orc_NodeName) const;
   const std::vector<C_OSCParamSetInterpretedNode> * GetInterpretedData(void) const;

   static C_OSCParamSetHandler & h_GetInstance(void);

private:
   static C_OSCParamSetHandler mhc_Singleton;
   std::vector<C_OSCParamSetRawNode> mc_RawNodes;
   std::vector<C_OSCParamSetInterpretedNode> mc_InterpretedNodes;

   C_OSCParamSetHandler(void);

   stw_types::sint32 m_LoadNodes(C_OSCXMLParser & orc_XMLParser, const bool oq_InterpretedDataOnly);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
