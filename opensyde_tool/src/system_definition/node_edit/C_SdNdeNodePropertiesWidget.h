//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for node properties (header)

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.01.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDENODEPROPERTIESWIDGET_H
#define C_SDNDENODEPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNdeNodePropertiesWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNdeNodePropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeNodePropertiesWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeNodePropertiesWidget(void);

   void InitStaticNames(void) const;
   void SelectName(void) const;
   void SetNodeId(const stw_types::uint32 ou32_NodeIndex);
   void SaveToData(void) const;

private:
   //Avoid call
   C_SdNdeNodePropertiesWidget(const C_SdNdeNodePropertiesWidget &);
   C_SdNdeNodePropertiesWidget & operator =(const C_SdNdeNodePropertiesWidget &);

   Ui::C_SdNdeNodePropertiesWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;
   static const stw_types::sint32 mhs32_PR_INDEX_DISABLED;
   static const stw_types::sint32 mhs32_PR_INDEX_ENABLED;

   void m_CheckNodeName(void);
   void m_TrimNodeName(void) const;
   void m_RegisterChange(void);
   void m_RegisterNameChange(void);
   void m_CheckComIfId(const stw_types::uint32 ou32_Row, const stw_types::uint32 ou32_Column) const;
   void m_IpAddressClick(const stw_types::uint32 ou32_Row, const stw_types::uint32 ou32_Column);
   void m_LoadFromData(void);
   void m_FlashloaderOptions(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName = "");
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
