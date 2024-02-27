//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user settings data (header)

   This class handles all actions concerning user settings.
   (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_USHANDLER_HPP
#define C_USHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include <QStringList>
#include <QFile>
#include <QPoint>
#include <QSize>
#include <QMap>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsHandler
{
public:
   enum E_SettingsSubSection
   {
      eNONE = 0,
      eDLLCONFIG,
      eADVANCED_SETTINGS,
      ePROGRESS_LOG
   };

   enum E_CanDllType
   {
      ePEAK = 0,
      eVECTOR = 1,
      eOTHER = 2
   };

   static C_UsHandler * h_GetInstance(void);
   static void h_Destroy(void);

   void SetDefault(void);

   //Get
   QPoint GetScreenPos(void) const;
   QSize GetAppSize(void) const;
   bool GetAppMaximized(void) const;
   int32_t GetSplitterSettingsHorizontal(void) const;
   bool GetSettingsAreExpanded(void) const;
   bool GetWiProgressExpanded(void) const;
   bool GetWiAdvSettExpanded(void) const;
   bool GetWiDllConfigExpanded(void) const;
   E_SettingsSubSection GetPopOpenSection(void) const;
   int32_t GetPropNodeId(void) const;
   int32_t GetPropBitrate(void) const;
   int32_t GetNodeCfgInterfaceIndex(void) const;
   int32_t GetFlashloaderResetWaitTime(void) const;
   int32_t GetRequestDownloadTimeout(void) const;
   int32_t GetTransferDataTimeout(void) const;
   QStringList GetLastKnownUpdateHexFilePaths(void) const;
   QString GetLastKnownUpdateHexFileLocation(void) const;
   E_CanDllType GetCanDllType(void) const;
   QString GetCanDllPath(void) const;
   QString GetCustomCanDllPath(void) const;

   //Set
   void SetScreenPos(const QPoint & orc_New);
   void SetAppSize(const QSize & orc_New);
   void SetAppMaximized(const bool oq_New);
   void SetSplitterSettingsHorizontal(const int32_t os32_New);
   void SetSettingsAreExpanded(const bool oq_New);
   void SetWiProgressExpanded(const bool oq_New);
   void SetWiAdvSettExpanded(const bool oq_New);
   void SetWiDllConfigExpanded(const bool oq_New);
   void SetPopOpenSection(const E_SettingsSubSection oe_PopOpenSection);
   void SetPropNodeId(const int32_t os32_NodeId);
   void SetPropBitrate(const int32_t os32_Bitrate);
   void SetNodeCfgInterfaceIndex(const int32_t os32_InterfaceIndex);
   void SetFlashloaderResetWaitTime(const int32_t & ors32_NewValue);
   void SetRequestDownloadTimeout(const int32_t & ors32_NewValue);
   void SetTransferDataTimeout(const int32_t & ors32_NewValue);
   void SetLastKnownUpdateHexFilePaths(const QStringList & orc_HexFilePaths);
   void SetLastKnownUpdateHexFileLocation(const QString & orc_NewPath);
   void SetCanDllType(const E_CanDllType oe_NewValue);
   void SetCustomCanDllPath(const QString & orc_NewValue);

   void Save(void) const;

   static int32_t h_GetParentFolder(const QString & orc_CompletePath, QString & orc_Parent,
                                    const bool & orq_CompletePathContainsFile);

private:
   static C_UsHandler * mhpc_Singleton;

   //Avoid call
   C_UsHandler(void);
   virtual ~C_UsHandler();
   C_UsHandler(const C_UsHandler &);
   C_UsHandler & operator =(const C_UsHandler &) &;

   const QString mc_IniPathAndName;           ///< Fix location of user settings
   QPoint mc_ScreenPos;                       ///< History of last known screen position
   QSize mc_AppSize;                          ///< History of last known openSyde window size
   bool mq_AppMaximized;                      ///< History of openSyde window state
   int32_t ms32_SplitterSettingsHorizontal;   ///< History of last known settings-splitter position x value
   bool mq_SettingsAreExpanded;               ///< History of last known settings state
                                              // (collapsed/expanded)
   bool mq_WiProgressExpanded;                ///< History of last known progress widget expanded state
   bool mq_WiAdvSettExpanded;                 ///< History of last known advanced settings widget expanded state
   bool mq_WiDllConfigExpanded;               ///< History of last known DLL configuration widget expanded
                                              // state
   E_SettingsSubSection me_PopOpenSection;    ///< History of last known expanded settings subsection
   int32_t ms32_PropNodeId;                   ///< History of last known Node ID in Properties widget
   int32_t ms32_PropBitrate;                  ///< History of last known Bus Bitrate in Properties widget
   int32_t ms32_NodeCfgInterfaceIndex;        ///< History of last known connected interface in Node Config dialog
   int32_t ms32_FlashloaderResetWaitTime;     ///< History of last known Flashloader Reset Wait Time
   int32_t ms32_RequestDownloadTimeout;       ///< History of last known Request Download Timeout
   int32_t ms32_TransferDataTimeout;          ///< History of last known Transfer Data Timeout
   QStringList mc_HexFilePaths;               ///< History of last known Hex file paths
   QString mc_LastKnownUpdateHexFileLocation; ///< History of last known Hex file location
   E_CanDllType me_CanDllType;                ///< History of last used CAN DLL type
   QString mc_CustomCanDllPath;               ///< History of last used custom CAN DLL path
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
