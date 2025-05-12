//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for general node properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEPROPERTIES_HPP
#define C_OSCNODEPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include "C_SclString.hpp"
#include "C_OscNodeStwFlashloaderSettings.hpp"
#include "C_OscNodeComInterfaceSettings.hpp"
#include "C_OscNodeOpenSydeServerSettings.hpp"
#include "C_OscDeviceDefinition.hpp"
#include "C_OscNodeCodeExportSettings.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeProperties
{
public:
   C_OscNodeProperties(void);
   virtual ~C_OscNodeProperties(void);

   //lint -sem(stw::opensyde_core::C_OscNodeProperties::Initialize,initializer)
   void Initialize(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   const C_OscNodeComInterfaceSettings * GetComInterface(const C_OscSystemBus::E_Type oe_InterfaceType,
                                                         const uint8_t ou8_InterfaceNumber) const;
   void SetComInterface(const C_OscNodeComInterfaceSettings & orc_ComInterface);
   void DisconnectComInterface(const C_OscSystemBus::E_Type oe_InterfaceType, const uint8_t ou8_InterfaceNumber);
   void CreateComInterfaces(const C_OscDeviceDefinition & orc_Device, const uint32_t ou32_SubDeviceIndex);

   std::vector<C_OscNodeComInterfaceSettings> GetCanInterfaces() const;
   std::vector<C_OscNodeComInterfaceSettings> GetEthernetInterfaces() const;

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

   stw::scl::C_SclString c_Name;                               ///< Unique name
   stw::scl::C_SclString c_Comment;                            ///< User comment
   E_DiagnosticServerProtocol e_DiagnosticServer;              ///< Selected diagnostic server
   E_FlashLoaderProtocol e_FlashLoader;                        ///< Selected flash loader
   std::vector<C_OscNodeComInterfaceSettings> c_ComInterfaces; ///< List of all present com interfaces
   C_OscNodeOpenSydeServerSettings c_OpenSydeServerSettings;   ///< Optional openSYDE server settings
   C_OscNodeStwFlashloaderSettings c_StwFlashloaderSettings;   ///< Optional STW flashloader settings
   C_OscNodeCodeExportSettings c_CodeExportSettings;           ///< Optional code export settings
   bool q_XappSupport;                                         ///< Flag to indicate X_App support of node

private:
   std::vector<C_OscNodeComInterfaceSettings> m_GetInterfaces(const C_OscSystemBus::E_Type oe_Type) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
