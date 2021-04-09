//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class widget for use case specific widgets

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGUSECASEWIDGET_H
#define C_NAGUSECASEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QVector>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagToolBarButtonProperties
{
public:
   QString c_ButtonText;
   QString c_ToolTipHeading;
   QString c_ToolTipContent;
};

class C_NagUseCaseWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagUseCaseWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_NagUseCaseWidget(void);

   virtual void SetParentHook(QWidget * const opc_Parent);

   QVector<C_NagToolBarButtonProperties> GetVecToolBarFuncNames(void) const;

   virtual void UserInputFunc(const stw_types::uint32 ou32_FuncNumber);
   virtual void Save(void);
   virtual void OpenDetail(const stw_types::sint32 os32_Index, const stw_types::sint32 os32_SubIndex,
                           const stw_types::sint32 os32_SubSubIndex, const stw_types::sint32 os32_SubSubSubIndex,
                           const stw_types::sint32 os32_Flag);
   virtual bool PrepareToClose(void);

   virtual void SetSubMode(const stw_types::sint32 os32_SubMode, const stw_types::uint32 ou32_Index,
                           const stw_types::uint32 ou32_Flag);
   virtual bool GlobalUserKeyPress(QKeyEvent * const opc_Event);

   //Generic push button
   virtual void OnPushButtonIconPress(void);

   virtual void CallHelp(void) = 0;

   // save as
   void SaveAs(void);

   // open color pciker
   void OpenColorPicker(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigShowUserInputFunc(const stw_types::uint32 ou32_FuncNumber, const bool oq_Show);
   void SigEnableUserInputFunc(const stw_types::uint32 ou32_FuncNumber, const bool oq_Enable);
   void SigShowBackgroundOfUserInputFunc(const stw_types::uint32 ou32_FuncNumber, const bool oq_Enable);
   void SigSetIconForUserInputFunc(const stw_types::uint32 ou32_FuncNumber, const QIcon & orc_Icon);
   void SigSetToolTipForUserInputFunc(const stw_types::uint32 ou32_FuncNumber, const QString & orc_ToolTipHeading,
                                      const QString & orc_ToolTipContent);
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                      const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubItemName,
                      const stw_types::uint32 ou32_Flag, const bool oq_ChangeUseCase = false);
   void SigDataChanged(const bool oq_Changed, const bool oq_All, const stw_types::sint32 os32_Mode,
                       const stw_types::sint32 os32_SubMode = 0, const stw_types::uint32 ou32_Index = 0U);
   void SigErrorChanged(const stw_types::uint32 ou32_Index);
   void SigStartViewClicked(void);
   void SigTriggerUpdateTitle(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName,
                       const bool oq_CombineItemAndSubSubName);
   void SigSetInteractionWidget(QWidget * const opc_Widget) const;
   void SigSearch(void);
   void SigNewUserSettings(void);
   void SigBlockDragAndDrop(const bool oq_Block);

protected:
   QVector<C_NagToolBarButtonProperties> mc_VecUserInputFuncNames;

private:
   //Avoid call
   C_NagUseCaseWidget(const C_NagUseCaseWidget &);
   C_NagUseCaseWidget & operator =(const C_NagUseCaseWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
