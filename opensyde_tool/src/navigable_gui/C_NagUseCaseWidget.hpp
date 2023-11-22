//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class widget for use case specific widgets

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGUSECASEWIDGET_HPP
#define C_NAGUSECASEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QVector>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui
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
   ~C_NagUseCaseWidget(void) override;

   virtual void SetParentHook(QWidget * const opc_Parent);

   QVector<C_NagToolBarButtonProperties> GetVecToolBarFuncNames(void) const;

   virtual void UserInputFunc(const uint32_t ou32_FuncNumber);
   virtual void Save(void);
   virtual void OpenDetail(const int32_t os32_Index, const int32_t os32_SubIndex, const int32_t os32_SubSubIndex,
                           const int32_t os32_SubSubSubIndex, const int32_t os32_Flag);
   virtual bool PrepareToClose(void);

   virtual void SetSubMode(const int32_t os32_SubMode, const uint32_t ou32_Index, const uint32_t ou32_Flag);
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
   void SigShowUserInputFunc(const uint32_t ou32_FuncNumber, const bool oq_Show);
   void SigEnableUserInputFunc(const uint32_t ou32_FuncNumber, const bool oq_Enable);
   void SigShowBackgroundOfUserInputFunc(const uint32_t ou32_FuncNumber, const bool oq_Enable);
   void SigSetIconForUserInputFunc(const uint32_t ou32_FuncNumber, const QIcon & orc_Icon);
   void SigSetToolTipForUserInputFunc(const uint32_t ou32_FuncNumber, const QString & orc_ToolTipHeading,
                                      const QString & orc_ToolTipContent);
   void SigChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode, const uint32_t ou32_Index,
                      const QString & orc_Name, const QString & orc_SubSubItemName, const uint32_t ou32_Flag,
                      const bool oq_ChangeUseCase = false);
   void SigDataChanged(const bool oq_Changed, const bool oq_All, const int32_t os32_Mode,
                       const int32_t os32_SubMode = 0, const uint32_t ou32_Index = 0U);
   void SigErrorChanged(const uint32_t ou32_Index);
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
   C_NagUseCaseWidget & operator =(const C_NagUseCaseWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
