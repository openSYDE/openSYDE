//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for upper toolbar with dynamic buttons

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGTOOLBARWIDGET_H
#define C_NAGTOOLBARWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QPixmap>
#include <QVector>

#include "C_NagUseCaseWidget.h"

#include "C_OgePubToolBar.h"
#include "C_OgeGbxTransparent.h"
#include "C_NagToolBarSearchResults.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagToolBarWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagToolBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagToolBarWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_NagToolBarWidget();

   void SetDarkTheme(void);
   void SetLightTheme(void);

   void InitText(void) const;
   void ConfigureButtons(C_NagUseCaseWidget * const opc_UseCaseWidget);
   void ResetButtons(void);

   void SetTopWidget(QWidget * const opc_Widget);

   void HideAllButtonsAndStoreState(void);
   void RestoreAllButtonsVisibility(void);

   void ShowSearch(const bool oq_Active);
   void Search(void) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;
   //lint -restore

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                      const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubItemName,
                      const stw_types::uint32 ou32_Flag = 0U, const bool oq_ChangeUseCase = false);
   void SigOpenDetail(const stw_types::sint32 os32_Index, const stw_types::sint32 os32_SubIndex,
                      const stw_types::sint32 os32_SubSubIndex, const stw_types::sint32 os32_SubSubSubIndex,
                      const stw_types::sint32 os32_Flag);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_NagToolBarWidget(const C_NagToolBarWidget &);
   C_NagToolBarWidget & operator =(const C_NagToolBarWidget &);

   Ui::C_NagToolBarWidget * mpc_Ui;

   C_NagUseCaseWidget * mpc_ActUseCaseWidget;
   QVector<stw_opensyde_gui_elements::C_OgePubToolBar *> mc_VecButtons;
   QVector<QWidget *> mc_VecGroupBox;
   QVector<bool> mc_VecGroupBoxVisible;

   static const stw_types::sintn mhsn_SPACER_SIZE;

   stw_opensyde_gui::C_NagToolBarSearchResults mc_SearchResultWidget;
   bool mq_FocusInSearchResultWidget;
   bool mq_ShowSearch;

   // slots
   void m_BtnClicked(const stw_types::uint32 ou32_Index);
   void m_TriggerHelp(void);
   void m_ShowBtn(const stw_types::uint32 ou32_Index, const bool oq_Show) const;
   void m_EnableBtn(const stw_types::uint32 ou32_Index, const bool oq_Enable) const;
   void m_ShowBackgroundOfBtn(const stw_types::uint32 ou32_Index, const bool oq_Enable) const;
   void m_SetIconForBtn(const stw_types::uint32 ou32_Index, const QIcon & orc_Icon) const;
   void m_SetToolTipForBtn(const stw_types::uint32 ou32_Index, const QString & orc_ToolTipHeading,
                           const QString & orc_ToolTipContent) const;

   void m_ShowSearchTree(void);
   void m_RepositionSearchTree(void);
   void m_CancelSearch(void);
   void m_ChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                     const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubItemName,
                     const stw_types::uint32 ou32_Flag);
   // slots for C_SdSearchTreeWidget
   void m_Hide(void);
   void m_FocusOut(void);
   static stw_types::sint32 mh_GetButtonStringWidth(const QString & orc_Text);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
