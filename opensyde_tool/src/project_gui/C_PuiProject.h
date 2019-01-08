//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Complete project information (header)

   Complete project information (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUIPROJECT_H
#define C_PUIPROJECT_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include "C_OSCProject.h"
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiProject :
   public stw_opensyde_core::C_OSCProject
{
public:
   stw_types::sint32 Save(const bool oq_ForceSaveAll = false);
   stw_types::sint32 Load(void);
   bool HasHashChanged(void) const;
   void SetPath(const QString & orc_Path);
   static QString h_GetDefaultPath(void);
   QString GetPath(void) const;
   QString GetFolderPath(void) const;
   bool GetSwitchUseCaseFlag(void) const;
   static C_PuiProject * h_GetInstance(void);
   static void h_Destroy(void);
   static stw_types::uint64 h_GetProjectSize(const QString & orc_ProjectPath);
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
   virtual ~C_PuiProject(void);

   stw_types::uint32 m_CalcHashProject(void) const;
   static void mh_AdaptProjectPathToSystemViews(const QString & orc_ProjectPath, QString & orc_SystemViewsPath);
   void m_InitialProjectLoad(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
