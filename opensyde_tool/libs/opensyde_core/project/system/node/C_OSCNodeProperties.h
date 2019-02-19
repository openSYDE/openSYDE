//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for general node properties (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCNODEPROPERTIES_H
#define C_OSCNODEPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include "CSCLString.h"
#include "C_OSCNodeStwFlashloaderSettings.h"
#include "C_OSCNodeComInterfaceSettings.h"
#include "C_OSCNodeOpenSydeServerSettings.h"
#include "C_OSCDeviceDefinition.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeProperties
{
public:
   C_OSCNodeProperties(void);
   virtual ~C_OSCNodeProperties(void);

   //lint -sem(stw_opensyde_core::C_OSCNodeProperties::Initialize,initializer)
   void Initialize(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   const C_OSCNodeComInterfaceSettings * GetComInterface(const C_OSCSystemBus::E_Type oe_InterfaceType,
                                                         const stw_types::uint8 ou8_InterfaceNumber) const;
   void SetComInterface(const C_OSCNodeComInterfaceSettings & orc_ComInterface);
   void DisconnectComInterface(const C_OSCSystemBus::E_Type oe_InterfaceType,
                               const stw_types::uint8 ou8_InterfaceNumber);
   void CreateComInterfaces(const C_OSCDeviceDefinition & orc_Device);

   ///possible types of diagnostic protocols
   enum E_DiagnosticServerProtocol
   {
      eDS_NONE,     ///< No diagnostic server available
      eDS_KEFEX,    ///< Diagnostic server protocol type KEFEX
      eDS_OPEN_SYDE ///< Diagnostic server protocol type openSYDE
   };

   ///possible types of flashloader protocols
   enum E_FlashLoaderProtocol
   {
      eFL_NONE,     ///< No Flash loader available
      eFL_STW,      ///< Flash loader protocol type STW Flashloader
      eFL_OPEN_SYDE ///< Flash loader protocol type openSYDE
   };

   stw_scl::C_SCLString c_Name;                                ///< Unique name
   stw_scl::C_SCLString c_Comment;                             ///< User comment
   E_DiagnosticServerProtocol e_DiagnosticServer;              ///< Selected diagnostic server
   E_FlashLoaderProtocol e_FlashLoader;                        ///< Selected flash loader
   std::vector<C_OSCNodeComInterfaceSettings> c_ComInterfaces; ///< List of all present com interfaces
   C_OSCNodeOpenSYDEServerSettings c_OpenSYDEServerSettings;   ///< Optional openSYDE server settings
   C_OSCNodeStwFlashloaderSettings c_STWFlashloaderSettings;   ///< Optional STW flashloader settings
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
