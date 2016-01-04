#include "Disassembly.h"
#include "Configuration.h"
#include "MainWindow.h"

Disassembly::Disassembly(QWidget* parent) : AbstractTableView(parent)
{
    mMemPage = new MemoryPage(0, 0);

    mInstBuffer.clear();

    historyClear();

    SelectionData_t data;
    memset(&data, 0, sizeof(SelectionData_t));
    mSelection = data;

    mCipRva = 0;
    mIsRunning = false;

    mHighlightToken.text = "";
    mHighlightingMode = false;

    int maxModuleSize = (int)ConfigUint("Disassembler", "MaxModuleSize");
    Config()->writeUints();

    mDisasm = new QBeaEngine(maxModuleSize);
    mDisasm->UpdateConfig();

    mIsLastInstDisplayed = false;

    mGuiState = Disassembly::NoState;

    // Update fonts immediately because they are used in calculations
    updateFonts();

    setRowCount(mMemPage->getSize());

    addColumnAt(getCharWidth() * 2 * sizeof(dsint) + 8, "", false); //address
    addColumnAt(getCharWidth() * 2 * 12 + 8, "", false); //bytes
    addColumnAt(getCharWidth() * 40, "", false); //disassembly
    addColumnAt(20, "", false); //comments

    setShowHeader(false); //hide header

    backgroundColor = ConfigColor("DisassemblyBackgroundColor");

    // Slots
//    connect(Bridge::getBridge(), SIGNAL(repaintGui()), this, SLOT(reloadData()));
//    connect(Bridge::getBridge(), SIGNAL(updateDump()), this, SLOT(reloadData()));
//    connect(Bridge::getBridge(), SIGNAL(dbgStateChanged(DBGSTATE)), this, SLOT(debugStateChangedSlot(DBGSTATE)));

    Initialize();
}

void Disassembly::updateColors()
{
    AbstractTableView::updateColors();
    backgroundColor = ConfigColor("DisassemblyBackgroundColor");

    mInstructionHighlightColor = ConfigColor("InstructionHighlightColor");
    mSelectionColor = ConfigColor("DisassemblySelectionColor");
    mCipBackgroundColor = ConfigColor("DisassemblyCipBackgroundColor");
    mCipColor = ConfigColor("DisassemblyCipColor");
    mBreakpointBackgroundColor = ConfigColor("DisassemblyBreakpointBackgroundColor");
    mBreakpointColor = ConfigColor("DisassemblyBreakpointColor");
    mHardwareBreakpointBackgroundColor = ConfigColor("DisassemblyHardwareBreakpointBackgroundColor");
    mHardwareBreakpointColor = ConfigColor("DisassemblyHardwareBreakpointColor");
    mBookmarkBackgroundColor = ConfigColor("DisassemblyBookmarkBackgroundColor");
    mBookmarkColor = ConfigColor("DisassemblyBookmarkColor");
    mLabelColor = ConfigColor("DisassemblyLabelColor");
    mLabelBackgroundColor = ConfigColor("DisassemblyLabelBackgroundColor");
    mSelectedAddressBackgroundColor = ConfigColor("DisassemblySelectedAddressBackgroundColor");
    mSelectedAddressColor = ConfigColor("DisassemblySelectedAddressColor");
    mAddressBackgroundColor = ConfigColor("DisassemblyAddressBackgroundColor");
    mAddressColor = ConfigColor("DisassemblyAddressColor");
    mBytesColor = ConfigColor("DisassemblyBytesColor");
    mModifiedBytesColor = ConfigColor("DisassemblyModifiedBytesColor");
    mRestoredBytesColor = ConfigColor("DisassemblyRestoredBytesColor");
    mAutoCommentColor = ConfigColor("DisassemblyAutoCommentColor");
    mAutoCommentBackgroundColor = ConfigColor("DisassemblyAutoCommentBackgroundColor");
    mCommentColor = ConfigColor("DisassemblyCommentColor");
    mCommentBackgroundColor = ConfigColor("DisassemblyCommentBackgroundColor");
    mUnconditionalJumpLineColor = ConfigColor("DisassemblyUnconditionalJumpLineColor");
    mConditionalJumpLineTrueColor = ConfigColor("DisassemblyConditionalJumpLineTrueColor");
    mConditionalJumpLineFalseColor = ConfigColor("DisassemblyConditionalJumpLineFalseColor");
    mLoopColor = ConfigColor("DisassemblyLoopColor");
    mFunctionColor = ConfigColor("DisassemblyFunctionColor");

    CapstoneTokenizer::UpdateColors();
    mDisasm->UpdateConfig();
}

void Disassembly::updateFonts()
{
    setFont(ConfigFont("Disassembly"));
}

/************************************************************************************
                            Reimplemented Functions
************************************************************************************/
/**
 * @brief       This method has been reimplemented. It returns the string to paint or paints it
 *              by its own.
 *
 * @param[in]   painter     Pointer to the painter that allows painting by its own
 * @param[in]   rowBase     Index of the top item (Table offset)
 * @param[in]   rowOffset   Index offset starting from rowBase
 * @param[in]   col         Column index
 * @param[in]   x           Rectangle x
 * @param[in]   y           Rectangle y
 * @param[in]   w           Rectangle width
 * @param[in]   h           Rectangle heigth
 *
 * @return      String to paint.
 */
QString Disassembly::paintContent(QPainter* painter, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
    Q_UNUSED(rowBase);

    if(mHighlightingMode)
    {
        QPen pen(mInstructionHighlightColor);
        pen.setWidth(2);
        painter->setPen(pen);
        QRect rect = viewport()->rect();
        rect.adjust(1, 1, -1, -1);
        painter->drawRect(rect);
    }
    dsint wRVA = mInstBuffer.at(rowOffset).rva;
    bool wIsSelected = isSelected(&mInstBuffer, rowOffset);

    // Highlight if selected
    if(wIsSelected)
        painter->fillRect(QRect(x, y, w, h), QBrush(mSelectionColor));

    switch(col)
    {
    case 0: // Draw address (+ label)
    {
        //char label[MAX_LABEL_SIZE] = "";
        //dsint cur_addr = rvaToVa(mInstBuffer.at(rowOffset).rva);
        dsint cur_addr = (mInstBuffer.at(rowOffset).rva);
        //QString addrText = getAddrText(cur_addr, label);
        QString addrText = QString("%1").arg(cur_addr, 8, 16, QChar('0'));

//        BPXTYPE bpxtype = DbgGetBpxTypeAt(cur_addr);
//        bool isbookmark = DbgGetBookmarkAt(cur_addr);
//        if(mInstBuffer.at(rowOffset).rva == mCipRva && !mIsRunning) //cip + not running
//        {
//            painter->fillRect(QRect(x, y, w, h), QBrush(mCipBackgroundColor));
//            if(!isbookmark) //no bookmark
//            {
//                if(bpxtype & bp_normal) //normal breakpoint
//                {
//                    QColor & bpColor = mBreakpointBackgroundColor;
//                    if(!bpColor.alpha()) //we don't want transparent text
//                        bpColor = mBreakpointColor;
//                    if(bpColor == mCipBackgroundColor)
//                        bpColor = mCipColor;
//                    painter->setPen(bpColor);
//                }
//                else if(bpxtype & bp_hardware) //hardware breakpoint only
//                {
//                    QColor hwbpColor = mHardwareBreakpointBackgroundColor;
//                    if(!hwbpColor.alpha()) //we don't want transparent text
//                        hwbpColor = mHardwareBreakpointColor;
//                    if(hwbpColor == mCipBackgroundColor)
//                        hwbpColor = mCipColor;
//                    painter->setPen(hwbpColor);
//                }
//                else //no breakpoint
//                {
//                    painter->setPen(mCipColor);
//                }
//            }
//            else //bookmark
//            {
//                QColor bookmarkColor = mBookmarkBackgroundColor;
//                if(!bookmarkColor.alpha()) //we don't want transparent text
//                    bookmarkColor = mBookmarkColor;
//                if(bookmarkColor == mCipBackgroundColor)
//                    bookmarkColor = mCipColor;
//                painter->setPen(bookmarkColor);
//            }
//        }
//        else //non-cip address
//        {
//            if(!isbookmark) //no bookmark
//            {
//                if(*label) //label
//                {
//                    if(bpxtype == bp_none) //label only
//                    {
//                        painter->setPen(mLabelColor); //red -> address + label text
//                        painter->fillRect(QRect(x, y, w, h), QBrush(mLabelBackgroundColor)); //fill label background
//                    }
//                    else //label+breakpoint
//                    {
//                        if(bpxtype & bp_normal) //label + normal breakpoint
//                        {
//                            painter->setPen(mBreakpointColor);
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
//                        }
//                        else if(bpxtype & bp_hardware) //label + hardware breakpoint only
//                        {
//                            painter->setPen(mHardwareBreakpointColor);
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill ?
//                        }
//                        else //other cases -> do as normal
//                        {
//                            painter->setPen(mLabelColor); //red -> address + label text
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mLabelBackgroundColor)); //fill label background
//                        }
//                    }
//                }
//                else //no label
//                {
//                    if(bpxtype == bp_none) //no label, no breakpoint
//                    {
//                        QColor background;
//                        if(wIsSelected)
//                        {
//                            background = mSelectedAddressBackgroundColor;
//                            painter->setPen(mSelectedAddressColor); //black address (DisassemblySelectedAddressColor)
//                        }
//                        else
//                        {
//                            background = mAddressBackgroundColor;
//                            painter->setPen(mAddressColor); //DisassemblyAddressColor
//                        }
//                        if(background.alpha())
//                            painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background
//                    }
//                    else //breakpoint only
//                    {
//                        if(bpxtype & bp_normal) //normal breakpoint
//                        {
//                            painter->setPen(mBreakpointColor);
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
//                        }
//                        else if(bpxtype & bp_hardware) //hardware breakpoint only
//                        {
//                            painter->setPen(mHardwareBreakpointColor);
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill red
//                        }
//                        else //other cases (memory breakpoint in disassembly) -> do as normal
//                        {
//                            QColor background;
//                            if(wIsSelected)
//                            {
//                                background = mSelectedAddressBackgroundColor;
//                                painter->setPen(mSelectedAddressColor); //black address (DisassemblySelectedAddressColor)
//                            }
//                            else
//                            {
//                                background = mAddressBackgroundColor;
//                                painter->setPen(mAddressColor);
//                            }
//                            if(background.alpha())
//                                painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill background
//                        }
//                    }
//                }
//            }
//            else //bookmark
//            {
//                if(*label) //label + bookmark
//                {
//                    if(bpxtype == bp_none) //label + bookmark
//                    {
//                        painter->setPen(mLabelColor); //red -> address + label text
//                        painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill label background
//                    }
//                    else //label+breakpoint+bookmark
//                    {
//                        QColor color = mBookmarkBackgroundColor;
//                        if(!color.alpha()) //we don't want transparent text
//                            color = mAddressColor;
//                        painter->setPen(color);
//                        if(bpxtype & bp_normal) //label + bookmark + normal breakpoint
//                        {
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
//                        }
//                        else if(bpxtype & bp_hardware) //label + bookmark + hardware breakpoint only
//                        {
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill ?
//                        }
//                    }
//                }
//                else //bookmark, no label
//                {
//                    if(bpxtype == bp_none) //bookmark only
//                    {
//                        painter->setPen(mBookmarkColor); //black address
//                        painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill bookmark color
//                    }
//                    else //bookmark + breakpoint
//                    {
//                        QColor color = mBookmarkBackgroundColor;
//                        if(!color.alpha()) //we don't want transparent text
//                            color = mAddressColor;
//                        painter->setPen(color);
//                        if(bpxtype & bp_normal) //bookmark + normal breakpoint
//                        {
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mBreakpointBackgroundColor)); //fill red
//                        }
//                        else if(bpxtype & bp_hardware) //bookmark + hardware breakpoint only
//                        {
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mHardwareBreakpointBackgroundColor)); //fill red
//                        }
//                        else //other cases (bookmark + memory breakpoint in disassembly) -> do as normal
//                        {
//                            painter->setPen(mBookmarkColor); //black address
//                            painter->fillRect(QRect(x, y, w, h), QBrush(mBookmarkBackgroundColor)); //fill bookmark color
//                        }
//                    }
//                }
//            }
//        }
        painter->drawText(QRect(x + 4, y , w - 4 , h), Qt::AlignVCenter | Qt::AlignLeft, addrText);
    }
    break;

    case 1: //draw bytes (TODO: some spaces between bytes)
    {
        //draw functions
        //dsint cur_addr = rvaToVa(mInstBuffer.at(rowOffset).rva);
        Function_t funcType;
//        FUNCTYPE funcFirst = DbgGetFunctionTypeAt(cur_addr);
//        FUNCTYPE funcLast = DbgGetFunctionTypeAt(cur_addr + mInstBuffer.at(rowOffset).length - 1);
//        if(funcLast == FUNC_END)
//            funcFirst = funcLast;
//        switch(funcFirst)
//        {
//        case FUNC_SINGLE:
//            funcType = Function_single;
//            break;
//        case FUNC_NONE:
//            funcType = Function_none;
//            break;
//        case FUNC_BEGIN:
//            funcType = Function_start;
//            break;
//        case FUNC_MIDDLE:
//            funcType = Function_middle;
//            break;
//        case FUNC_END:
//            funcType = Function_end;
//            break;
//        }
        funcType = Function_none;
        int funcsize = paintFunctionGraphic(painter, x, y, funcType, false);

        //draw jump arrows
        int jumpsize = paintJumpsGraphic(painter, x + funcsize, y - 1, wRVA); //jump line

        //draw bytes
        QList<RichTextPainter::CustomRichText_t> richBytes;
        RichTextPainter::CustomRichText_t space;
        space.highlight = false;
        space.flags = RichTextPainter::FlagNone;
        space.text = " ";
        RichTextPainter::CustomRichText_t curByte;
        curByte.highlight = false;
        curByte.flags = RichTextPainter::FlagColor;
        auto dump = mInstBuffer.at(rowOffset).dump;
        for(int i = 0; i < dump.size(); i++)
        {
            if(i)
                richBytes.push_back(space);
            auto byte = (unsigned char)dump.at(i);
            curByte.text = QString("%1").arg(byte, 2, 16, QChar('0')).toUpper();
//            DBGPATCHINFO patchInfo;
//            if(DbgFunctions()->PatchGetEx(cur_addr + i, &patchInfo))
//            {
//                auto log = QString().sprintf("oldbyte: %02X, newbyte: %02X, byte: %02X\n", patchInfo.oldbyte, patchInfo.newbyte, byte);
//                GuiAddLogMessage(log.toUtf8().constData());
//                curByte.textColor = byte == patchInfo.newbyte ? mModifiedBytesColor : mRestoredBytesColor;
//            }
//            else
                curByte.textColor = mBytesColor;
            richBytes.push_back(curByte);
        }
        RichTextPainter::paintRichText(painter, x, y, getColumnWidth(col), getRowHeight(), jumpsize + funcsize, &richBytes, getCharWidth());
    }
    break;

    case 2: //draw disassembly (with colours needed)
    {
        //dsint cur_addr = rvaToVa(mInstBuffer.at(rowOffset).rva);
        int loopsize = 0;
        //int depth = 0;

//        while(1) //paint all loop depths
//        {
//            LOOPTYPE loopType = DbgGetLoopTypeAt(cur_addr, depth);
//            if(loopType == LOOP_NONE)
//                break;
//            Function_t funcType;
//            switch(loopType)
//            {
//            case LOOP_BEGIN:
//                funcType = Function_start;
//                break;
//            case LOOP_ENTRY:
//                funcType = Function_loop_entry;
//                break;
//            case LOOP_MIDDLE:
//                funcType = Function_middle;
//                break;
//            case LOOP_END:
//                funcType = Function_end;
//                break;
//            default:
//                break;
//            }
//            loopsize += paintFunctionGraphic(painter, x + loopsize, y, funcType, true);
//            depth++;
//        }

        QList<RichTextPainter::CustomRichText_t> richText;

        auto & token = mInstBuffer[rowOffset].tokens;
        if(mHighlightToken.text.length())
            CapstoneTokenizer::TokenToRichText(token, richText, &mHighlightToken);
        else
            CapstoneTokenizer::TokenToRichText(token, richText, 0);
        int xinc = 4;
        RichTextPainter::paintRichText(painter, x + loopsize, y, getColumnWidth(col) - loopsize, getRowHeight(), xinc, &richText, getCharWidth());
        token.x = x + loopsize + xinc;
    }
    break;

//    case 3: //draw comments
//    {
//        char comment[MAX_COMMENT_SIZE] = "";
//        if(DbgGetCommentAt(rvaToVa(mInstBuffer.at(rowOffset).rva), comment))
//        {
//            QString commentText;
//            QColor backgroundColor;
//            if(comment[0] == '\1') //automatic comment
//            {
//                painter->setPen(mAutoCommentColor);
//                backgroundColor = mAutoCommentBackgroundColor;
//                commentText = QString(comment + 1);
//            }
//            else //user comment
//            {
//                painter->setPen(mCommentColor);
//                backgroundColor = mCommentBackgroundColor;
//                commentText = comment;
//            }

//            int width = getCharWidth() * commentText.length() + 4;
//            if(width > w)
//                width = w;
//            if(width)
//                painter->fillRect(QRect(x + 2, y, width, h), QBrush(backgroundColor)); //fill comment color
//            painter->drawText(QRect(x + 4, y , w - 4 , h), Qt::AlignVCenter | Qt::AlignLeft, commentText);
//        }
//    }
//    break;
    }
    return "";
}


/************************************************************************************
                            Mouse Management
************************************************************************************/
/**
 * @brief       This method has been reimplemented. It manages the following actions:
 *               - Multi-rows selection
 *
 * @param[in]   event       Mouse event
 *
 * @return      Nothing.
 */
void Disassembly::mouseMoveEvent(QMouseEvent* event)
{
    //qDebug() << "Disassembly::mouseMoveEvent";

    bool wAccept = true;

    if(mGuiState == Disassembly::MultiRowsSelectionState)
    {
        //qDebug() << "State = MultiRowsSelectionState";

        if((transY(event->y()) >= 0) && (transY(event->y()) <= this->getTableHeigth()))
        {
            int wI = getIndexOffsetFromY(transY(event->y()));

            if(mMemPage->getSize() > 0)
            {
                // Bound
                wI = wI >= mInstBuffer.size() ? mInstBuffer.size() - 1 : wI;
                wI = wI < 0 ? 0 : wI;

                dsint wRowIndex = mInstBuffer.at(wI).rva;
                dsint wInstrSize = getInstructionRVA(wRowIndex, 1) - wRowIndex - 1;

                if(wRowIndex < getRowCount())
                {
                    setSingleSelection(getInitialSelection());
                    expandSelectionUpTo(getInstructionRVA(getInitialSelection(), 1) - 1);
                    if(wRowIndex > getInitialSelection()) //select down
                        expandSelectionUpTo(wRowIndex + wInstrSize);
                    else
                        expandSelectionUpTo(wRowIndex);

                    updateViewport();

                    wAccept = false;
                }
            }
        }
    }

    if(wAccept == true)
        AbstractTableView::mouseMoveEvent(event);
}


/**
 * @brief       This method has been reimplemented. It manages the following actions:
 *               - Multi-rows selection
 *
 * @param[in]   event       Mouse event
 *
 * @return      Nothing.
 */
void Disassembly::mousePressEvent(QMouseEvent* event)
{
    bool wAccept = false;

//    if(DbgIsDebugging() && ((event->buttons() & Qt::LeftButton) != 0) && ((event->buttons() & Qt::RightButton) == 0))
//    {
//        if(getGuiState() == AbstractTableView::NoState)
//        {
//            if(mHighlightingMode)
//            {
//                if(getColumnIndexFromX(event->x()) == 2) //click in instruction column
//                {
//                    int rowOffset = getIndexOffsetFromY(transY(event->y()));
//                    if(rowOffset < mInstBuffer.size())
//                    {
//                        CapstoneTokenizer::SingleToken token;
//                        if(CapstoneTokenizer::TokenFromX(mInstBuffer.at(rowOffset).tokens, token, event->x(), getCharWidth()))
//                        {
//                            if(CapstoneTokenizer::IsHighlightableToken(token) && !CapstoneTokenizer::TokenEquals(&token, &mHighlightToken))
//                                mHighlightToken = token;
//                            else
//                            {
//                                mHighlightToken = CapstoneTokenizer::SingleToken();
//                            }
//                        }
//                        else
//                        {
//                            mHighlightToken = CapstoneTokenizer::SingleToken();
//                        }
//                    }
//                }
//                else
//                {
//                    mHighlightToken = CapstoneTokenizer::SingleToken();
//                }
//            }
//            else if(event->y() > getHeaderHeight())
//            {
//                dsint wRowIndex = getInstructionRVA(getTableOffset(), getIndexOffsetFromY(transY(event->y())));
//                dsint wInstrSize = getInstructionRVA(wRowIndex, 1) - wRowIndex - 1;

//                if(wRowIndex < getRowCount())
//                {
//                    if(!(event->modifiers() & Qt::ShiftModifier)) //SHIFT pressed
//                        setSingleSelection(wRowIndex);
//                    if(getSelectionStart() > wRowIndex) //select up
//                    {
//                        setSingleSelection(getInitialSelection());
//                        expandSelectionUpTo(getInstructionRVA(getInitialSelection(), 1) - 1);
//                        expandSelectionUpTo(wRowIndex);
//                    }
//                    else //select down
//                    {
//                        setSingleSelection(getInitialSelection());
//                        expandSelectionUpTo(wRowIndex + wInstrSize);
//                    }

//                    mGuiState = Disassembly::MultiRowsSelectionState;

//                    updateViewport();

//                    wAccept = true;
//                }
//            }
//        }
//    }

    if(wAccept == false)
        AbstractTableView::mousePressEvent(event);
}


/**
 * @brief       This method has been reimplemented. It manages the following actions:
 *               - Multi-rows selection
 *
 * @param[in]   event       Mouse event
 *
 * @return      Nothing.
 */
void Disassembly::mouseReleaseEvent(QMouseEvent* event)
{
    bool wAccept = true;

    if((event->buttons() & Qt::LeftButton) == 0)
    {
        if(mGuiState == Disassembly::MultiRowsSelectionState)
        {
            mGuiState = Disassembly::NoState;

            updateViewport();

            wAccept = false;
        }
    }

    if(wAccept == true)
        AbstractTableView::mouseReleaseEvent(event);
}


/************************************************************************************
                            Keyboard Management
************************************************************************************/
/**
 * @brief       This method has been reimplemented. It processes the Up/Down key events.
 *
 * @param[in]   event       Key event
 *
 * @return      Nothing.
 */
void Disassembly::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();

    if(key == Qt::Key_Up || key == Qt::Key_Down)
    {
        dsint botRVA = getTableOffset();
        dsint topRVA = getInstructionRVA(getTableOffset(), getNbrOfLineToPrint() - 1);

        bool expand = false;
        if(event->modifiers() & Qt::ShiftModifier) //SHIFT pressed
            expand = true;

        if(key == Qt::Key_Up)
            selectPrevious(expand);
        else
            selectNext(expand);

        if(getSelectionStart() < botRVA)
        {
            setTableOffset(getSelectionStart());
        }
        else if(getSelectionEnd() >= topRVA)
        {
            setTableOffset(getInstructionRVA(getSelectionEnd(), -getNbrOfLineToPrint() + 2));
        }

        updateViewport();
    }
    else if(key == Qt::Key_Return || key == Qt::Key_Enter)
    {
        // TODO
//        duint dest = DbgGetBranchDestination(rvaToVa(getInitialSelection()));
//        if(!dest)
//            return;
//        QString cmd = "disasm " + QString("%1").arg(dest, sizeof(dsint) * 2, 16, QChar('0')).toUpper();
//        DbgCmdExec(cmd.toUtf8().constData());
    }
    else
        AbstractTableView::keyPressEvent(event);
}

/************************************************************************************
                            ScrollBar Management
 ***********************************************************************************/
/**
 * @brief       This method has been reimplemented. It realigns the slider on real instructions except
 *              when the type is QAbstractSlider::SliderNoAction. This type (QAbstractSlider::SliderNoAction)
 *              is used to force the disassembling at a specific RVA.
 *
 * @param[in]   type      Type of action
 * @param[in]   value     Old table offset
 * @param[in]   delta     Scrollbar value delta compared to the previous state
 *
 * @return      Return the value of the new table offset.
 */
dsint Disassembly::sliderMovedHook(int type, dsint value, dsint delta)
{
    // QAbstractSlider::SliderNoAction is used to disassembe at a specific address
    if(type == QAbstractSlider::SliderNoAction)
        return value + delta;

    // If it's a slider action, disassemble one instruction back and one instruction next in order to be aligned on a real instruction
    if(type == QAbstractSlider::SliderMove)
    {
        dsint wNewValue = 0;

        if(value + delta > 0)
        {
            wNewValue = getInstructionRVA(value + delta, -1);
            wNewValue = getInstructionRVA(wNewValue, 1);
        }

        return wNewValue;
    }

    // For other actions, disassemble according to the delta
    return getInstructionRVA(value, delta);
}


/************************************************************************************
                            Jumps Graphic
************************************************************************************/
/**
 * @brief       This method paints the graphic for jumps.
 *
 * @param[in]   painter     Pointer to the painter that allows painting by its own
 * @param[in]   x           Rectangle x
 * @param[in]   y           Rectangle y
 * @param[in]   addr        RVA of address to process
 *
 * @return      Nothing.
 */
int Disassembly::paintJumpsGraphic(QPainter* painter, int x, int y, dsint addr)
{
    dsint selHeadRVA = mSelection.fromIndex;
    dsint rva = addr;
    Instruction_t instruction = DisassembleAt(selHeadRVA);
    auto branchType = instruction.branchType;

    GraphicDump_t wPict = GD_Nothing;

    if(branchType != Instruction_t::None)
    {
        dsint destRVA = instruction.branchDestination;

        dsint base = mMemPage->getBase();
        if(destRVA >= base && destRVA < base + (dsint)mMemPage->getSize())
        {
            destRVA -= (dsint)mMemPage->getBase();

            if(destRVA < selHeadRVA)
            {
                if(rva == destRVA)
                    wPict = GD_HeadFromBottom;
                else if(rva > destRVA && rva < selHeadRVA)
                    wPict = GD_Vert;
                else if(rva == selHeadRVA)
                    wPict = GD_FootToTop;
            }
            else if(destRVA > selHeadRVA)
            {
                if(rva == selHeadRVA)
                    wPict = GD_FootToBottom;
                else if(rva > selHeadRVA && rva < destRVA)
                    wPict = GD_Vert;
                else if(rva == destRVA)
                    wPict = GD_HeadFromTop;
            }
        }
    }

    //bool bIsExecute = DbgIsJumpGoingToExecute(rvaToVa(instruction.rva));

    if(branchType == Instruction_t::Unconditional) //unconditional
    {
        painter->setPen(mUnconditionalJumpLineColor);
    }
    else
    {
//        if(bIsExecute)
//            painter->setPen(mConditionalJumpLineTrueColor);
//        else
            painter->setPen(mConditionalJumpLineFalseColor);
    }

    if(wPict == GD_Vert)
    {
        painter->drawLine(x, y, x, y + getRowHeight());
    }
    else if(wPict == GD_FootToBottom)
    {
        painter->drawLine(x, y + getRowHeight() / 2, x + 5, y + getRowHeight() / 2);
        painter->drawLine(x, y + getRowHeight() / 2, x, y + getRowHeight());
    }
    else if(wPict == GD_FootToTop)
    {
        painter->drawLine(x, y + getRowHeight() / 2, x + 5, y + getRowHeight() / 2);
        painter->drawLine(x, y, x, y + getRowHeight() / 2);
    }
    else if(wPict == GD_HeadFromBottom)
    {
        QPoint wPoints[] =
        {
            QPoint(x + 3, y + getRowHeight() / 2 - 2),
            QPoint(x + 5, y + getRowHeight() / 2),
            QPoint(x + 3, y + getRowHeight() / 2 + 2),
        };

        painter->drawLine(x, y + getRowHeight() / 2, x + 5, y + getRowHeight() / 2);
        painter->drawLine(x, y + getRowHeight() / 2, x, y + getRowHeight());
        painter->drawPolyline(wPoints, 3);
    }
    else if(wPict == GD_HeadFromTop)
    {
        QPoint wPoints[] =
        {
            QPoint(x + 3, y + getRowHeight() / 2 - 2),
            QPoint(x + 5, y + getRowHeight() / 2),
            QPoint(x + 3, y + getRowHeight() / 2 + 2),
        };

        painter->drawLine(x, y + getRowHeight() / 2, x + 5, y + getRowHeight() / 2);
        painter->drawLine(x, y, x, y + getRowHeight() / 2);
        painter->drawPolyline(wPoints, 3);
    }

    return 7;
}

/************************************************************************************
                            Function Graphic
************************************************************************************/
/**
 * @brief       This method paints the graphic for functions/loops.
 *
 * @param[in]   painter     Pointer to the painter that allows painting by its own
 * @param[in]   x           Rectangle x
 * @param[in]   y           Rectangle y
 * @param[in]   funcType    Type of drawing to make
 *
 * @return      Width of the painted data.
 */

int Disassembly::paintFunctionGraphic(QPainter* painter, int x, int y, Function_t funcType, bool loop)
{
    if(loop && funcType == Function_none)
        return 0;
    if(loop)
        painter->setPen(QPen(mLoopColor, 2)); //thick black line
    else
        painter->setPen(QPen(mFunctionColor, 2)); //thick black line
    int height = getRowHeight();
    int x_add = 5;
    int y_add = 4;
    int end_add = 2;
    int line_width = 3;
    if(loop)
    {
        end_add = -1;
        x_add = 4;
    }
    switch(funcType)
    {
    case Function_single:
    {
        if(loop)
            y_add = height / 2 + 1;
        painter->drawLine(x + x_add + line_width, y + y_add, x + x_add, y + y_add);
        painter->drawLine(x + x_add, y + y_add, x + x_add, y + height - y_add - 1);
        if(loop)
            y_add = height / 2 - 1;
        painter->drawLine(x + x_add, y + height - y_add, x + x_add + line_width, y + height - y_add);
    }
    break;

    case Function_start:
    {
        if(loop)
            y_add = height / 2 + 1;
        painter->drawLine(x + x_add + line_width, y + y_add, x + x_add, y + y_add);
        painter->drawLine(x + x_add, y + y_add, x + x_add, y + height);
    }
    break;

    case Function_middle:
    {
        painter->drawLine(x + x_add, y, x + x_add, y + height);
    }
    break;

    case Function_loop_entry:
    {
        int trisize = 2;
        int y_start = (height - trisize * 2) / 2 + y;
        painter->drawLine(x + x_add, y_start, x + trisize + x_add, y_start + trisize);
        painter->drawLine(x + trisize + x_add, y_start + trisize, x + x_add, y_start + trisize * 2);

        painter->drawLine(x + x_add, y, x + x_add, y_start - 1);
        painter->drawLine(x + x_add, y_start + trisize * 2 + 2, x + x_add, y + height);
    }
    break;

    case Function_end:
    {
        if(loop)
            y_add = height / 2 - 1;
        painter->drawLine(x + x_add, y, x + x_add, y + height - y_add);
        painter->drawLine(x + x_add, y + height - y_add, x + x_add + line_width, y + height - y_add);
    }
    break;

    case Function_none:
    {

    }
    break;
    }
    return x_add + line_width + end_add;
}


/************************************************************************************
                            Instructions Management
 ***********************************************************************************/
/**
 * @brief       Returns the RVA of count-th instructions before the given instruction RVA.
 *
 * @param[in]   rva         Instruction RVA
 * @param[in]   count       Instruction count
 *
 * @return      RVA of count-th instructions before the given instruction RVA.
 */
dsint Disassembly::getPreviousInstructionRVA(dsint rva, duint count)
{
    QByteArray wBuffer;
    dsint wBottomByteRealRVA;
    dsint wVirtualRVA;
    dsint wMaxByteCountToRead ;

    wBottomByteRealRVA = (dsint)rva - 16 * (count + 3);
    wBottomByteRealRVA = wBottomByteRealRVA < 0 ? 0 : wBottomByteRealRVA;

    wVirtualRVA = (dsint)rva - wBottomByteRealRVA;

    wMaxByteCountToRead = wVirtualRVA + 1 + 16;
    wBuffer.resize(wMaxByteCountToRead);

    mMemPage->read(reinterpret_cast<byte_t*>(wBuffer.data()), wBottomByteRealRVA, wMaxByteCountToRead);

    dsint addr = mDisasm->DisassembleBack(reinterpret_cast<byte_t*>(wBuffer.data()), 0,  wMaxByteCountToRead, wVirtualRVA, count);

    addr += rva - wVirtualRVA;

    return addr;
}


/**
 * @brief       Returns the RVA of count-th instructions after the given instruction RVA.
 *
 * @param[in]   rva         Instruction RVA
 * @param[in]   count       Instruction count
 *
 * @return      RVA of count-th instructions after the given instruction RVA.
 */
dsint Disassembly::getNextInstructionRVA(dsint rva, duint count)
{
    QByteArray wBuffer;
    dsint wVirtualRVA = 0;
    dsint wRemainingBytes;
    dsint wMaxByteCountToRead;
    dsint wNewRVA;

    if(mMemPage->getSize() < (duint)rva)
        return rva;
    wRemainingBytes = mMemPage->getSize() - rva;

    wMaxByteCountToRead = 16 * (count + 1);
    wMaxByteCountToRead = wRemainingBytes > wMaxByteCountToRead ? wMaxByteCountToRead : wRemainingBytes;
    wBuffer.resize(wMaxByteCountToRead);

    mMemPage->read(reinterpret_cast<byte_t*>(wBuffer.data()), rva, wMaxByteCountToRead);

    wNewRVA = mDisasm->DisassembleNext(reinterpret_cast<byte_t*>(wBuffer.data()), 0,  wMaxByteCountToRead, wVirtualRVA, count);
    wNewRVA += rva;

    return wNewRVA;
}


/**
 * @brief       Returns the RVA of count-th instructions before/after (depending on the sign) the given instruction RVA.
 *
 * @param[in]   rva         Instruction RVA
 * @param[in]   count       Instruction count
 *
 * @return      RVA of count-th instructions before/after the given instruction RVA.
 */
dsint Disassembly::getInstructionRVA(dsint index, dsint count)
{
    dsint wAddr = 0;

    if(count == 0)
        wAddr = index;
    if(count < 0)
        wAddr = getPreviousInstructionRVA(index, qAbs(count));
    else if(count > 0)
        wAddr = getNextInstructionRVA(index, qAbs(count));


    if(wAddr < 0)
        wAddr = 0;
    else if(wAddr > getRowCount() - 1)
        wAddr = getRowCount() - 1;

    return wAddr;
}


/**
 * @brief       Disassembles the instruction at the given RVA.
 *
 * @param[in]   rva     RVA of instruction to disassemble
 *
 * @return      Return the disassembled instruction.
 */
Instruction_t Disassembly::DisassembleAt(dsint rva)
{
    QByteArray wBuffer;
    dsint base = mMemPage->getBase();
    dsint wMaxByteCountToRead = 16 * 2;

    // Bounding
    //TODO: fix problems with negative sizes
    dsint size = getSize();
    if(!size)
        size = rva;

    wMaxByteCountToRead = wMaxByteCountToRead > (size - rva) ? (size - rva) : wMaxByteCountToRead;

    wBuffer.resize(wMaxByteCountToRead);

    mMemPage->read(reinterpret_cast<byte_t*>(wBuffer.data()), rva, wMaxByteCountToRead);

    return mDisasm->DisassembleAt(reinterpret_cast<byte_t*>(wBuffer.data()), wMaxByteCountToRead, 0, base, rva);
}


/**
 * @brief       Disassembles the instruction count instruction afterc the instruction at the given RVA.
 *              Count can be positive or negative.
 *
 * @param[in]   rva     RVA of reference instruction
 * @param[in]   count   Number of instruction
 *
 * @return      Return the disassembled instruction.
 */
Instruction_t Disassembly::DisassembleAt(dsint rva, dsint count)
{
    rva = getNextInstructionRVA(rva, count);
    return DisassembleAt(rva);
}


/************************************************************************************
                                Selection Management
************************************************************************************/
void Disassembly::expandSelectionUpTo(dsint to)
{
    if(to < mSelection.firstSelectedIndex)
    {
        mSelection.fromIndex = to;
    }
    else if(to > mSelection.firstSelectedIndex)
    {
        mSelection.toIndex = to;
    }
    else if(to == mSelection.firstSelectedIndex)
    {
        setSingleSelection(to);
    }
}

void Disassembly::setSingleSelection(dsint index)
{
    mSelection.firstSelectedIndex = index;
    mSelection.fromIndex = index;
    mSelection.toIndex = index;
    emit selectionChanged(rvaToVa(index));
}


dsint Disassembly::getInitialSelection()
{
    return mSelection.firstSelectedIndex;
}

dsint Disassembly::getSelectionSize()
{
    return mSelection.toIndex - mSelection.fromIndex;
}

dsint Disassembly::getSelectionStart()
{
    return mSelection.fromIndex;
}

dsint Disassembly::getSelectionEnd()
{
    return mSelection.toIndex;
}

void Disassembly::selectNext(bool expand)
{
    dsint wAddr;
    dsint wStart = getInstructionRVA(getSelectionStart(), 1) - 1;
    if(expand)
    {
        if(getSelectionEnd() == getInitialSelection() && wStart != getSelectionEnd()) //decrease down
        {
            wAddr = getInstructionRVA(getSelectionStart(), 1);
            expandSelectionUpTo(wAddr);
        }
        else //expand down
        {
            wAddr = getSelectionEnd() + 1;
            dsint wInstrSize = getInstructionRVA(wAddr, 1) - wAddr - 1;
            expandSelectionUpTo(wAddr + wInstrSize);
        }
    }
    else //select next instruction
    {
        wAddr = getSelectionEnd() + 1;
        setSingleSelection(wAddr);
        dsint wInstrSize = getInstructionRVA(wAddr, 1) - wAddr - 1;
        expandSelectionUpTo(wAddr + wInstrSize);
    }
}


void Disassembly::selectPrevious(bool expand)
{
    dsint wAddr;
    dsint wStart = getInstructionRVA(getSelectionStart(), 1) - 1;
    if(expand)
    {
        if(getSelectionStart() == getInitialSelection() && wStart != getSelectionEnd()) //decrease up
        {
            wAddr = getInstructionRVA(getSelectionEnd() + 1, -2);
            dsint wInstrSize = getInstructionRVA(wAddr, 1) - wAddr - 1;
            expandSelectionUpTo(wAddr + wInstrSize);
        }
        else //expand up
        {
            wAddr = getInstructionRVA(wStart + 1, -2);
            expandSelectionUpTo(wAddr);
        }
    }
    else
    {
        wAddr = getInstructionRVA(getSelectionStart(), -1);
        setSingleSelection(wAddr);
        dsint wInstrSize = getInstructionRVA(wAddr, 1) - wAddr - 1;
        expandSelectionUpTo(wAddr + wInstrSize);
    }
}


bool Disassembly::isSelected(dsint base, dsint offset)
{
    dsint wAddr = base;

    if(offset < 0)
        wAddr = getPreviousInstructionRVA(getTableOffset(), offset);
    else if(offset > 0)
        wAddr = getNextInstructionRVA(getTableOffset(), offset);

    if(wAddr >= mSelection.fromIndex && wAddr <= mSelection.toIndex)
        return true;
    else
        return false;
}


bool Disassembly::isSelected(QList<Instruction_t>* buffer, int index)
{
    if(buffer->size() > 0 && index >= 0 && index < buffer->size())
    {
        if((dsint)buffer->at(index).rva >= mSelection.fromIndex && (dsint)buffer->at(index).rva <= mSelection.toIndex)
            return true;
        else
            return false;
    }
    else
    {
        return false;
    }
}

duint Disassembly::getSelectedVa()
{
    // Wrapper around commonly used code:
    // Converts the selected index to a valid virtual address
    return rvaToVa(getInitialSelection());
}

/************************************************************************************
                         Update/Reload/Refresh/Repaint
************************************************************************************/

void Disassembly::prepareDataCount(dsint wRVA, int wCount, QList<Instruction_t>* instBuffer)
{
    instBuffer->clear();
    Instruction_t wInst;
    for(int wI = 0; wI < wCount; wI++)
    {
        wInst = DisassembleAt(wRVA);
        instBuffer->append(wInst);
        wRVA += wInst.length;
    }
}

void Disassembly::prepareDataRange(dsint startRva, dsint endRva, QList<Instruction_t>* instBuffer)
{
    if(startRva == endRva)
        prepareDataCount(startRva, 1, instBuffer);
    else
    {
        int wCount = 0;
        dsint addr = startRva;
        while(addr <= endRva)
        {
            addr = getNextInstructionRVA(addr, 1);
            wCount++;
        }
        prepareDataCount(startRva, wCount, instBuffer);
    }
}

void Disassembly::prepareData()
{
    dsint wViewableRowsCount = getViewableRowsCount();

    dsint wAddrPrev = getTableOffset();
    dsint wAddr = wAddrPrev;

    int wCount = 0;

    for(int wI = 0; wI < wViewableRowsCount && getRowCount() > 0; wI++)
    {
        wAddrPrev = wAddr;
        wAddr = getNextInstructionRVA(wAddr, 1);

        if(wAddr == wAddrPrev)
        {
            break;
        }

        wCount++;
    }

    setNbrOfLineToPrint(wCount);

    prepareDataCount(getTableOffset(), wCount, &mInstBuffer);
}

void Disassembly::reloadData()
{
    emit selectionChanged(rvaToVa(mSelection.firstSelectedIndex));
    AbstractTableView::reloadData();
}


/************************************************************************************
                        Public Methods
************************************************************************************/
duint Disassembly::rvaToVa(dsint rva)
{
    return mMemPage->va(rva);
}

void Disassembly::disassembleAt(dsint parVA, dsint parCIP, bool history, dsint newTableOffset)
{
    dsint wBase = mMemPage->getBase();
    dsint wSize = mMemPage->getSize();

    if(!wBase || !wSize)
        return;
    dsint wRVA = parVA - wBase;
    dsint wCipRva = parCIP - wBase;

    HistoryData_t newHistory;

    //VA history
    if(history)
    {
        //truncate everything right from the current VA
        if(mVaHistory.size() && mCurrentVa < mVaHistory.size() - 1) //mCurrentVa is not the last
            mVaHistory.erase(mVaHistory.begin() + mCurrentVa + 1, mVaHistory.end());

        //NOTE: mCurrentVa always points to the last entry of the list

        //add the currently selected address to the history
        dsint selectionVA = rvaToVa(getInitialSelection()); //currently selected VA
        dsint selectionTableOffset = getTableOffset();
        if(selectionVA && mVaHistory.size() && mVaHistory.last().va != selectionVA) //do not have 2x the same va in a row
        {
            if(mVaHistory.size() >= 1024) //max 1024 in the history
            {
                mCurrentVa--;
                mVaHistory.erase(mVaHistory.begin()); //remove the oldest element
            }
            mCurrentVa++;
            newHistory.va = selectionVA;
            newHistory.tableOffset = selectionTableOffset;
            //newHistory.windowTitle = MainWindow::windowTitle;
            mVaHistory.push_back(newHistory);
        }
    }

    // Set base and size (Useful when memory page changed)
    mMemPage->setAttributes(wBase, wSize);

    if(mRvaDisplayEnabled && mMemPage->getBase() != mRvaDisplayPageBase)
        mRvaDisplayEnabled = false;

    setRowCount(wSize);

    setSingleSelection(wRVA);               // Selects disassembled instruction
    dsint wInstrSize = getInstructionRVA(wRVA, 1) - wRVA - 1;
    expandSelectionUpTo(wRVA + wInstrSize);

    //set CIP rva
    mCipRva = wCipRva;

    if(newTableOffset == -1) //nothing specified
    {
        // Update table offset depending on the location of the instruction to disassemble
        if(mInstBuffer.size() > 0 && wRVA >= (dsint)mInstBuffer.first().rva && wRVA < (dsint)mInstBuffer.last().rva)
        {
            int wI;
            bool wIsAligned = false;

            // Check if the new RVA is aligned on an instruction from the cache (buffer)
            for(wI = 0; wI < mInstBuffer.size(); wI++)
            {
                if(mInstBuffer.at(wI).rva == wRVA)
                {
                    wIsAligned = true;
                    break;
                }
            }

            if(wIsAligned == true)
            {
                updateViewport();
            }
            else
            {
                setTableOffset(wRVA);
            }
        }
        else if(mInstBuffer.size() > 0 && wRVA == (dsint)mInstBuffer.last().rva)
        {
            setTableOffset(mInstBuffer.first().rva + mInstBuffer.first().length);
        }
        else
        {
            setTableOffset(wRVA);
        }

        if(history)
        {
            //new disassembled address
            newHistory.va = parVA;
            newHistory.tableOffset = getTableOffset();
            //newHistory.windowTitle = MainWindow::windowTitle;
            if(mVaHistory.size())
            {
                if(mVaHistory.last().va != parVA) //not 2x the same va in history
                {
                    if(mVaHistory.size() >= 1024) //max 1024 in the history
                    {
                        mCurrentVa--;
                        mVaHistory.erase(mVaHistory.begin()); //remove the oldest element
                    }
                    mCurrentVa++;
                    mVaHistory.push_back(newHistory); //add a va to the history
                }
            }
            else //the list is empty
                mVaHistory.push_back(newHistory);
        }
    }
    else //specified new table offset
        setTableOffset(newTableOffset);

    /*
    //print history
    if(history)
    {
        QString strList = "";
        for(int i=0; i<mVaHistory.size(); i++)
            strList += QString().sprintf("[%d]:%p,%p\n", i, mVaHistory.at(i).va, mVaHistory.at(i).tableOffset);
        MessageBoxA(GuiGetWindowHandle(), strList.toUtf8().constData(), QString().sprintf("mCurrentVa=%d", mCurrentVa).toUtf8().constData(), MB_ICONINFORMATION);
    }
    */
    emit disassembledAt(parVA,  parCIP,  history,  newTableOffset);
    reloadData();

}

QList<Instruction_t>* Disassembly::instructionsBuffer()
{
    return &mInstBuffer;
}

const dsint Disassembly::currentEIP() const
{
    return mCipRva;
}


void Disassembly::disassembleAt(dsint parVA, dsint parCIP)
{
    disassembleAt(parVA, parCIP, true, -1);
}


void Disassembly::disassembleClear()
{
    mHighlightingMode = false;
    mHighlightToken = CapstoneTokenizer::SingleToken();
    historyClear();
    mMemPage->setAttributes(0, 0);
    setRowCount(0);
    reloadData();
}


//void Disassembly::debugStateChangedSlot(DBGSTATE state)
//{
//    switch(state)
//    {
//    case stopped:
//        disassembleClear();
//        break;
//    case paused:
//        mIsRunning = false;
//        break;
//    case running:
//        mIsRunning = true;
//        break;
//    default:
//        break;
//    }
//}

const dsint Disassembly::getBase() const
{
    return mMemPage->getBase();
}

dsint Disassembly::getSize()
{
    return mMemPage->getSize();
}

duint Disassembly::getTableOffsetRva()
{
    return mInstBuffer.size() ? mInstBuffer.at(0).rva : 0;

}

void Disassembly::historyClear()
{
    mVaHistory.clear(); //clear history for new targets
    mCurrentVa = 0;
}

void Disassembly::historyPrevious()
{
    if(!mCurrentVa || !mVaHistory.size()) //we are at the earliest history entry
        return;
    mCurrentVa--;
    disassembleAt(mVaHistory.at(mCurrentVa).va, rvaToVa(mCipRva), false, mVaHistory.at(mCurrentVa).tableOffset);

    // Update window title
    emit updateWindowTitle(mVaHistory.at(mCurrentVa).windowTitle);
}

void Disassembly::historyNext()
{
    int size = mVaHistory.size();
    if(!size || mCurrentVa >= mVaHistory.size() - 1) //we are at the newest history entry
        return;
    mCurrentVa++;
    disassembleAt(mVaHistory.at(mCurrentVa).va, rvaToVa(mCipRva), false, mVaHistory.at(mCurrentVa).tableOffset);

    // Update window title
    emit updateWindowTitle(mVaHistory.at(mCurrentVa).windowTitle);
}

bool Disassembly::historyHasPrevious()
{
    if(!mCurrentVa || !mVaHistory.size()) //we are at the earliest history entry
        return false;
    return true;
}

bool Disassembly::historyHasNext()
{
    int size = mVaHistory.size();
    if(!size || mCurrentVa >= mVaHistory.size() - 1) //we are at the newest history entry
        return false;
    return true;
}

QString Disassembly::getAddrText(dsint cur_addr, char label[MAX_LABEL_SIZE])
{
    QString addrText = "";
    if(mRvaDisplayEnabled) //RVA display
    {
        dsint rva = cur_addr - mRvaDisplayBase;
        if(rva == 0)
        {
#ifdef _WIN64
            addrText = "$ ==>            ";
#else
            addrText = "$ ==>    ";
#endif //_WIN64
        }
        else if(rva > 0)
        {
#ifdef _WIN64
            addrText = "$+" + QString("%1").arg(rva, -15, 16, QChar(' ')).toUpper();
#else
            addrText = "$+" + QString("%1").arg(rva, -7, 16, QChar(' ')).toUpper();
#endif //_WIN64
        }
        else if(rva < 0)
        {
#ifdef _WIN64
            addrText = "$-" + QString("%1").arg(-rva, -15, 16, QChar(' ')).toUpper();
#else
            addrText = "$-" + QString("%1").arg(-rva, -7, 16, QChar(' ')).toUpper();
#endif //_WIN64
        }
    }
    addrText += ToPtrString(cur_addr);
    char label_[MAX_LABEL_SIZE] = "";
//    if(DbgGetLabelAt(cur_addr, SEG_DEFAULT, label_)) //has label
//    {
//        char module[MAX_MODULE_SIZE] = "";
//        if(DbgGetModuleAt(cur_addr, module) && !QString(label_).startsWith("JMP.&"))
//            addrText += " <" + QString(module) + "." + QString(label_) + ">";
//        else
//            addrText += " <" + QString(label_) + ">";
//    }
//    else
        *label_ = 0;
    if(label)
        strcpy_s(label, MAX_LABEL_SIZE, label_);
    return addrText;
}
