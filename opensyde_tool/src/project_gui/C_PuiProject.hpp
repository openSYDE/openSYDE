//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete project information (header)

   Complete project information (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUIPROJECT_HPP
#define C_PUIPROJECT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "C_OscProject.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiProject :
   public stw::opensyde_core::C_OscProject
{
public:
   ~C_PuiProject(void) noexcept override;

   int32_t Save(const bool oq_ForceSaveAll = false, const bool oq_UseDeprecatedFileFormatV2 = false);
   int32_t SaveCurrentProjectForServiceMode(const QString & orc_FilePath, const QString & orc_Password,
                                            const std::vector<std::array<bool, 3> > & orc_ViewConfigs);
   int32_t Load(uint16_t * const opu16_FileVersion);
   bool HasHashChanged(void) const;
   void SetPath(const QString & orc_Path);
   bool IsPasswordNecessary(void);
   void SetPassword(const QString & orc_Password);
   QString GetPath(void) const;
   QString GetFolderPath(void) const;
   QString GetName(void) const;
   bool GetSwitchUseCaseFlag(void) const;
   static C_PuiProject * h_GetInstance(void);
   static void h_Destroy(void);
   static uint64_t h_GetProjectSize(const QString & orc_ProjectPath);
   int32_t PrepareLoadInitialProject(void);
   int32_t LoadInitialProject(uint16_t * const opu16_FileVersion, QString & orc_LoadedProject);
   void LoadEmpty(void);
   bool IsEmptyProject(void) const;
   static void h_HandlePendingEvents(void);
   static void h_AdaptProjectPathToSystemDefinition(const QString & orc_ProjectPath, QString & orc_SystemDefintionPath);
   int32_t SaveAsWithoutInternalChange(const QString & orc_FilePath, const bool oq_UseDeprecatedFileFormatV2);

private:
   static C_PuiProject * mhpc_Singleton;
   QString mc_Path;     ///< Currently active project path
   QString mc_Password; ///< In case of a service project the used password
   uint32_t mu32_CalculatedProjectHash;
   bool mq_SwitchToLastKnownUseCase;

   static const QString mhc_TEMP_FOLDER;
   static const QString mhc_SERVICE_PROJECT_EXTENSION;

   //Avoid calling
   C_PuiProject(void);

   uint32_t m_CalcHashProject(void) const;

   int32_t m_Save(const bool oq_ForceSaveAll, const bool oq_UseDeprecatedFileFormatV2);
   int32_t m_SaveServiceModeProject(const QString & orc_FilePath, const QString & orc_Password);
   int32_t m_LoadProject(uint16_t * const opu16_FileVersion);
   int32_t m_LoadServiceModeProject(const QString & orc_Password, uint16_t * const opu16_FileVersion);
   bool m_IsServiceModeProject(void) const;
   int32_t m_SaveAs(const QString & orc_FilePath, const bool oq_ForceSaveAll, const bool oq_UseDeprecatedFileFormatV2,
                    const bool oq_UpdateInternalState);

   static void mh_AdaptProjectPathToSystemViews(const QString & orc_ProjectPath, QString & orc_SystemViewsPath);
   static void mh_AdaptProjectPathToSystemViewsV1(const QString & orc_ProjectPath, QString & orc_SystemViewsPath);
   static void mh_AdaptProjectPathToSystemDefinitionV2(const QString & orc_ProjectPath,
                                                       QString & orc_SystemDefintionPath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
