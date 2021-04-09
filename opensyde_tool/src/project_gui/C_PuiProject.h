//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete project information (header)

   Complete project information (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIPROJECT_H
#define C_PUIPROJECT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "C_OSCProject.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiProject :
   public stw_opensyde_core::C_OSCProject
{
public:
   virtual ~C_PuiProject(void);

   stw_types::sint32 Save(const bool oq_ForceSaveAll = false, const bool oq_UseDeprecatedFileFormatV2 = false);
   stw_types::sint32 Load(stw_types::uint16 * const opu16_FileVersion);
   bool HasHashChanged(void) const;
   void SetPath(const QString & orc_Path);
   QString GetPath(void) const;
   QString GetFolderPath(void) const;
   QString GetName(void) const;
   bool GetSwitchUseCaseFlag(void) const;
   static C_PuiProject * h_GetInstance(void);
   static void h_Destroy(void);
   static stw_types::uint64 h_GetProjectSize(const QString & orc_ProjectPath);
   stw_types::sint32 LoadInitialProject(stw_types::uint16 * const opu16_FileVersion, QString & orc_LoadedProject);
   void LoadEmpty(void);
   bool IsEmptyProject(void) const;
   static void h_HandlePendingEvents(void);
   static void h_AdaptProjectPathToSystemDefinition(const QString & orc_ProjectPath, QString & orc_SystemDefintionPath);

private:
   static C_PuiProject * mhpc_Singleton;
   QString mc_Path; ///< Currently active project path
   stw_types::uint32 mu32_CalculatedProjectHash;
   bool mq_SwitchToLastKnownUseCase;

   //Avoid calling
   C_PuiProject(void);

   stw_types::uint32 m_CalcHashProject(void) const;
   static void mh_AdaptProjectPathToSystemViews(const QString & orc_ProjectPath, QString & orc_SystemViewsPath);
   static void mh_AdaptProjectPathToSystemViewsV1(const QString & orc_ProjectPath, QString & orc_SystemViewsPath);
   static void mh_AdaptProjectPathToSystemDefinitionV2(const QString & orc_ProjectPath,
                                                       QString & orc_SystemDefintionPath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
