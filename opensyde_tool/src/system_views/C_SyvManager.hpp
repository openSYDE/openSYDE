//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System views manager (add, delete, ...) (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVMANAGER_H
#define C_SYVMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvManager :
   public QObject
{
   Q_OBJECT

public:
   C_SyvManager(QObject * const opc_Parent = NULL);

   void AddView(const bool oq_SendSigChangeMode = true);
   void UpdateAllViewNames(void) const;
   void HandleInitialSystemView(void);
   void RenameView(const uint32_t ou32_Index, const QString & orc_Name) const;
   void MoveView(const uint32_t ou32_StartIndex, const uint32_t ou32_TargetIndex);
   void DuplicateSysView(const uint32_t ou32_Index);
   void DeleteSysView(const uint32_t ou32_Index, const int32_t os32_SelectedSubMode, const uint32_t ou32_SelectedIndex,
                      QWidget * const opc_Parent);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigReloadNaviBarSystemViewContent(void);
   void SigReloadNaviBarSystemViewNames(void) const;
   void SigChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode = 0, const uint32_t ou32_Index = 0U,
                      const QString & orc_Name = "", const QString & orc_SubSubMode = "", const uint32_t ou32_Flag = 0U,
                      const bool oq_ChangeUseCase = false);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
