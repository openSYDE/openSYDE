//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Max performance delegate (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETTREEDELEGATE_H
#define C_CAMMETTREEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>
#include <QPainter>
#include <QSvgRenderer>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_CamMetTreeDelegate(QObject * const opc_Parent = NULL);
   ~C_CamMetTreeDelegate(void) override;

   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;
   QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartAccept(void) const;
   void SigEndAccept(void) const;
   void SigStoreRenderer(const QString & orc_Path, QSvgRenderer * const opc_Renderer) const;

private:
   QMap<QString, QSvgRenderer *> mc_ActiveRenderers;
   static const QColor mhc_HIGHLIGHT_BACKGROUND_COLOR;
   static const QColor mhc_HIGHLIGHT_FOREGROUND_COLOR;
   static const QColor mhc_HIGHLIGHT_BORDER_COLOR;
   static const QColor mhc_DEFAULT_BACKGROUND_COLOR;
   static const QColor mhc_DEFAULT_BORDER_COLOR;
   static const QFont mhc_HIGHLIGHT_FONT;
   static const QFont mhc_DEFAULT_FONT;

   void m_PaintSelectedCellIcon(QPainter * const opc_Painter, const QRect & orc_CellRect, const QModelIndex & orc_Index,
                                const bool oq_Selected) const;
   void m_StoreRenderer(const QString & orc_Path, QSvgRenderer * const opc_Renderer);
   static bool mh_PaintChildCell(QPainter * const opc_Painter, const QRect & orc_CellRect,
                                 const QModelIndex & orc_Index, const bool oq_Selected);
   static std::vector<int32_t> mh_GetChildColWidths(const bool oq_IsThirdLayer);
   static std::vector<QFlags<Qt::AlignmentFlag> > mh_GetTopAlignmentFlags(void);
   static std::vector<QString> mh_GetTopSpaces(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
