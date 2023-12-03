// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc_vis (exe) -- amc: draw access path diagrams
// Exceptions: yes
// Source: cpp/amc/amc_vis.cpp
//
// Visualize ctype graph

#include "include/gen/amc_vis_gen.h"
#include "include/gen/amc_vis_gen.inl.h"


// -----------------------------------------------------------------------------

// schedule a before b
static void MakeABeforeB(amc_vis::Link &a, amc_vis::Link &b) {
    amc_vis::Linkdep &linkdep = amc_vis::linkdep_Alloc();
    linkdep.p_link_from     = &a;
    linkdep.p_link_to       = &b;
    vrfy(amc_vis::linkdep_XrefMaybe(linkdep), algo_lib::_db.errtext);
}

// -----------------------------------------------------------------------------

static void Main_RowLayout() {
    // count # incoming links for each link
    ind_beg(amc_vis::_db_link_curs, link, amc_vis::_db) {
        link.linkkey.colweight = zd_link_out_N(*link.p_node2) + zd_link_in_N(*link.p_node2);
        ind_beg(amc_vis::link_zd_linkdep_out_curs, linkdep, link) {
            linkdep.p_link_to->linkkey.n_link_in++;
        }ind_end;
    }ind_end;

    ind_beg(amc_vis::_db_link_curs, link, amc_vis::_db) {
        amc_vis::bh_link_Reheap(link);
    }ind_end;

    // topologically sort fields! create final sorted list of links -- linklist
    while (amc_vis::Link *link = amc_vis::bh_link_RemoveFirst()) {
        amc_vis::c_linklist_Insert(*link);
        // schedule rest sooner
        if (link == link->p_node2->c_top) {
            link->p_node2->c_bottom->linkkey.topbot -= 1;
            amc_vis::bh_link_Reheap(*link->p_node2->c_bottom);
        }
        // Schedule all links that lead to the same node sooner
        if (link == link->p_node2->c_top) {
            ind_beg(amc_vis::node_zd_link_in_curs, prior, *link->p_node2) if (prior.p_node1 == link->p_node1 && prior.p_node2 == link->p_node2 && bh_link_InBheapQ(prior)) {
                prior.linkkey.samecol -= 2;
                amc_vis::bh_link_Reheap(prior);
            }ind_end;
        }
        // reduce dependency count for target columns
        ind_beg(amc_vis::link_zd_linkdep_out_curs, from, *link) {
            from.p_link_to->linkkey.n_link_in--;
            amc_vis::bh_link_Reheap(*from.p_link_to);
        }ind_end;
    }

    // allocate each field to a row
    // rows will be created later
    int next_outrow = 0;
    ind_beg(amc_vis::_db_c_linklist_curs, link, amc_vis::_db) {
        next_outrow += 1;
        if (&link == link.p_node2->c_top) {
            next_outrow += 1;
        }
        // allocate row
        link.outrow = next_outrow;
        // skip one row after bottom item
        if (&link == link.p_node2->c_bottom) {
            next_outrow += 1;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static i32 ComputeXpos(amc_vis::FNode *node) {
    i32 xpos = 0;
    if (amc_vis::_db.cmdline.render) {// save time if not rendering...
        ind_beg(amc_vis::_db_node_curs, prior, amc_vis::_db) if (&prior != node) {
            ind_beg(amc_vis::node_zd_link_out_curs, link, prior) {
                if (link.outrow >= node->c_top->outrow && link.outrow <= node->c_bottom->outrow) {
                    xpos = i32_Max(xpos, prior.xpos + ch_N(link.label1));
                    xpos = i32_Max(xpos, prior.xpos + ch_N(link.label2));
                } else if (link.p_node1->c_top->outrow <= node->c_top->outrow && link.p_node2->c_bottom->outrow >= node->c_bottom->outrow) {
                    xpos = i32_Max(xpos, prior.xpos + 2);
                }
            }ind_end;
        }ind_end;
    }
    return xpos;
}

// -----------------------------------------------------------------------------

static void ShowCircular(amc_vis::FNode *node, int level) {
    ind_beg(amc_vis::node_zd_nodedep_in_curs,nodedep,*node) if (nodedep.p_node1->nodekey.n_ct_in>0){
        amc_vis::FNode *left = nodedep.p_node1;
        amc_vis::FNode *right = nodedep.p_node2;// node itself
        tempstr msg;
        char_PrintNTimes(' ',msg,level*4);
        msg << "amc_vis.circdep"
            <<Keyval("level", level)
            <<Keyval("left",left->node)
            <<Keyval("right",node->node)
            <<Keyval("field",tempstr()<<(nodedep.up ? right:left)->node<<"."<<nodedep.name)
            <<Keyval("reftype",nodedep.reftype)
            <<Keyval("left_nodekey", left->nodekey);
        prlog(msg);
        if (level<3) {
            ShowCircular(left,level+1);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static bool DepRefQ(amc_vis::FField &field) {
    return field.p_arg != field.p_ctype
        && field.reftype != dmmeta_Reftype_reftype_Global
        && field.reftype != dmmeta_Reftype_reftype_Regx
        && field.reftype != dmmeta_Reftype_reftype_RegxSql
        && field.reftype != dmmeta_Reftype_reftype_Hook;
}

// -----------------------------------------------------------------------------

static void Main_ColLayout() {
    // Column layout phase
    // create edge for each field (from source ctype to target ctype).
    ind_beg(amc_vis::_db_node_curs, node, amc_vis::_db) {
        ind_beg(amc_vis::ctype_c_field_curs, field, *node.p_ctype) {
            amc_vis::FNode *node_to = amc_vis::ind_node_Find(field.arg);
            if (node_to != NULL && !field.p_reftype->isnew && DepRefQ(field)) {
                amc_vis::FNodedep &nodedep = amc_vis::nodedep_Alloc();
                nodedep.name       = name_Get(field);
                nodedep.reftype    = field.reftype;
                nodedep.up         = field.p_reftype->up;
                nodedep.p_node1    = nodedep.up ? node_to : &node;
                nodedep.p_node2    = nodedep.up ? &node : node_to;
                verblog("amc_vis.nodedep"
                        <<Keyval("nodedep",tempstr()<<nodedep.p_node1->node<<"/"<<nodedep.p_node2->node)
                        <<Keyval("sourcefield",field.field));
                vrfy(amc_vis::nodedep_XrefMaybe(nodedep), algo_lib::_db.errtext);
            }
        }ind_end;
    }ind_end;

    // construct heap of entities with key=number of incoming edges
    // we don't assign the count to the link list because the count
    // will change but the list will stay the same.
    ind_beg(amc_vis::_db_node_curs, node, amc_vis::_db) {
        ind_beg(amc_vis::node_zd_nodedep_out_curs, nodedep, node) {
            nodedep.p_node2->nodekey.n_ct_in++;
            nodedep.p_node2->nodekey.prev_xpos = ComputeXpos(&node);
        }ind_end;
    }ind_end;

    ind_beg(amc_vis::_db_node_curs, node, amc_vis::_db) {
        amc_vis::bh_node_Reheap(node);
    }ind_end;

    // now allocate a column for each node
    // try all columns from left to right, find one where no overlap occurs
    // start at rightmost column belonging to one of the referrers.
    int nerr=0;
    while (amc_vis::FNode *node = amc_vis::bh_node_RemoveFirst()) {
        if (node->nodekey.n_ct_in > 0 && amc_vis::_db.cmdline.check) {
            ShowCircular(node,0);
            nerr++;
            prlog("");
        }
        node->xpos = ComputeXpos(node) + 4;
        // reduce dependency count for target columns
        // can this loop around???
        ind_beg(amc_vis::node_zd_nodedep_out_curs, nodedep, *node) if (amc_vis::bh_node_InBheapQ(*nodedep.p_node2)) {
            nodedep.p_node2->nodekey.n_ct_in--;
            nodedep.p_node2->nodekey.prev_xpos = ComputeXpos(nodedep.p_node2);
            amc_vis::bh_node_Reheap(*nodedep.p_node2);
        }ind_end;
    }

    if (nerr) {
        algo_lib::_db.exit_code=1;
        prlog("amc_vis.circular_dependency"
              <<Keyval("comment","Please examine above text and resolve circular dependencies (renaming Upptr->Ptr, etc)"));
    }
}

// -----------------------------------------------------------------------------

static void Main_Render() {
    // compute max # rows
    int max_rows = 0;
    int screen_wid = 0;
    ind_beg(amc_vis::_db_c_linklist_curs, link, amc_vis::_db) {
        max_rows = u32_Max(max_rows, link.outrow+1);
        screen_wid = i32_Max(screen_wid, link.p_node1->xpos + ch_N(link.label1) + 2);
        screen_wid = i32_Max(screen_wid, link.p_node2->xpos + ch_N(link.label2) + 2);
    }ind_end;

    // create rows:
    // each row is an array of chars.
    while (amc_vis::outrow_N() < max_rows) {
        amc_vis::Outrow &outrow = amc_vis::outrow_Alloc();
        vrfy(amc_vis::outrow_XrefMaybe(outrow), algo_lib::_db.errtext);
        amc_vis::text_Reserve(outrow, screen_wid);
        outrow.text_n = screen_wid;
        Fill(text_Getary(outrow), (unsigned char)' ');
    }

    // draw arrows.
    // these go left or right between columns
    ind_beg(amc_vis::_db_c_linklist_curs, link, amc_vis::_db) if (link.p_node2 != link.p_node1) {
        algo::aryptr<u8> line = text_Getary(amc_vis::outrow_qFind(link.outrow));

        bool rtol = link.p_node1->xpos > link.p_node2->xpos;
        char arrow = rtol ? '<' : '>';
        int left = link.p_node1->xpos;
        int right = link.p_node2->xpos;
        if (rtol) {
            left = link.p_node2->xpos+1;
            right = link.p_node1->xpos;
        }
        for (int i = left; i < right; i++) {
            line[i] = '-';
        }
        if (rtol) {
            line[left] = arrow;
        } else {
            line[right-1] = arrow;
        }
    }ind_end;

    // render boxes -- for each node, extending from opener to bottom fields
    ind_beg(amc_vis::_db_node_curs, node, amc_vis::_db) {
        amc_vis::Link &top = *node.c_top;
        amc_vis::Link &bot = *node.c_bottom;
        int left = node.xpos;

        // draw the box
        for (int y = top.outrow; y <= bot.outrow; y++) {
            amc_vis::Outrow &outrow = amc_vis::outrow_qFind(y);
            text_qFind(outrow, left) = y == top.outrow ? '/' : y == bot.outrow ? '-' : '|';
        }
    }ind_end;

    ind_beg(amc_vis::_db_c_linklist_curs, link, amc_vis::_db) {
        algo::aryptr<u8> line = text_Getary(amc_vis::outrow_qFind(link.outrow));

        // render left label
        if (link.p_node1 && link.p_node1 != link.p_node2) {
            int xleft = link.p_node1->xpos;
            strptr s = link.label1;
            frep_(i,elems_N(s)) {
                line[xleft + 1 + i] = s[i];
            }
        }
        // render right label
        if (link.p_node2) {
            int xleft = link.p_node2->xpos;
            strptr s = link.label2;
            frep_(i,elems_N(s)) {
                line[xleft + 2 + i] = s[i];
            }
        }
    }ind_end;


    ind_beg(amc_vis::_db_outrow_curs, outrow, amc_vis::_db) {
        prlog(ToStrPtr(text_Getary(outrow)));
    }ind_end;
}


//
// Generate dot file.
// Each node becomes a dot graph node.
// Each link (except
//
static void Main_GenerateDot() {
    cstring out;
    out << "digraph { \n";
    out << " rankdir=LR;\n";
    out << " ranksep=\"2.0\";\n";
    out << " splines=\"true\";\n";
    out << " size=\"50,30\";\n";
    out << " node [fontname=\"Courier\", fontcolor=\"#333388\"];\n";
    out << " edge [color=\"#333388\"];\n";

    ind_beg(amc_vis::_db_node_curs, node,amc_vis::_db) {
        out << "  \"" << node.node << "\" [\n";
        out << "   shape=plaintext, \n";
        out << "   label=<\n";
        out << "    <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" COLOR=\"#bbbbbb\">\n";

        // header -- name of the node
        out << "     <TR><TD PORT=";
        strptr_PrintDot(node.node, out);
        out << ">";
        strptr_PrintXml(node.node, out, false);
        out << "</TD></TR>\n";

        ind_beg(amc_vis::node_zd_link_out_curs, link, node) if (ch_N(link.label1) > 0) {
            strptr label = link.label1;//link.p_node1 == &node ? strptr(link.label1) : strptr(link.label2);
            out << "     <TR><TD PORT=";
            strptr_PrintDot(label, out);
            out << ">";
            strptr_PrintXml(label, out, false);
            out << "</TD></TR>\n";
        }ind_end;
        out << "    </TABLE>\n";
        out << "   >\n";
        out << " ];\n";
    }ind_end;

    ind_beg(amc_vis::_db_link_curs, link, amc_vis::_db) if (link.p_node1 != link.p_node2) {
        amc_vis::FNode *node1 = link.p_node1;
        amc_vis::FNode *node2 = link.p_node2;
        strptr label1 = link.label1;
        strptr label2 = link.label2;
        if (link.upptr) {
            algo::TSwap(node1,node2);
            algo::TSwap(label1,label2);
        }
        out << " ";
        strptr_PrintDot(node1->node, out);
        out << ":";
        strptr_PrintDot(label1, out);
        out << " -> ";
        strptr_PrintDot(node2->node, out);
        if (elems_N(label2) > 0 && node2->c_top != &link) {
            out << ":";
            strptr_PrintDot(label2, out);
        }
        out << "[";
        algo::ListSep ls(";");
        if (link.upptr) {
            out << "dir=back";
        }
        out << "]";
        // if (elems_N(link.label)) {
        //     out << "[label=";
        //     strptr_PrintDot(link.label, out);
        //     out << "]";
        // }
        out << ";";
        out << eol;
    }ind_end;

    out << "}\n";
    verblog(out);
    StringToFile(out, amc_vis::_db.cmdline.dot, algo_FileFlags__throw);
    tempstr svg;
    svg << StripExt(amc_vis::_db.cmdline.dot) << ".svg";
    SysCmd(tempstr() << "dot -Tsvg -o "<<svg<<" "<<amc_vis::_db.cmdline.dot, FailokQ(true), DryrunQ(false));
    prerr("amc_vis.dot"
          <<Keyval("out_dot",amc_vis::_db.cmdline.dot)
          <<Keyval("out_svg",svg));
}

// -----------------------------------------------------------------------------

void amc_vis::Main() {
    // mark ctypes as selected
    ind_beg(amc_vis::_db_ctype_curs, ctype, amc_vis::_db) if (Regx_Match(amc_vis::_db.cmdline.ctype, strptr(ctype.ctype))) {
        zd_select_Insert(ctype);
    }ind_end;

    // select all reachable ctypes
    if (amc_vis::_db.cmdline.xref) {
        ind_beg(amc_vis::_db_ctype_curs, ctype, amc_vis::_db) if (Regx_Match(amc_vis::_db.cmdline.ctype, strptr(ctype.ctype))) {
            ind_beg(amc_vis::ctype_c_field_curs, field, ctype) {
                bool select  = amc_vis::_db.cmdline.xns || ns_Get(*field.p_arg) == ns_Get(ctype);
                if (select) {
                    zd_select_Insert(*field.p_arg);
                }
            }ind_end;
        }ind_end;
    }

    // determine which ctypes are inputs
    ind_beg(amc_vis::_db_field_curs, field, amc_vis::_db) if (field.c_finput) {
        field.p_arg->isinput = true;
    }ind_end;

    // select all reachable ctypes
    if (amc_vis::_db.cmdline.noinput) {
        ind_beg(amc_vis::_db_ctype_curs, ctype, amc_vis::_db) if (zd_select_InLlistQ(ctype) && ctype.isinput) {
            zd_select_Remove(ctype);
        }ind_end;
    }

    // create a node for each ctype in this namespace.
    ind_beg(amc_vis::_db_zd_select_curs, ctype, amc_vis::_db) {
        amc_vis::FNode &node = amc_vis::node_Alloc();
        node.node       = ctype.ctype;
        node.nodekey.idx = -node.rowid;
        node.label      = ctype.ctype;
        node.p_ctype    = &ctype;
        vrfy(amc_vis::node_XrefMaybe(node), algo_lib::_db.errtext);
    }ind_end;

    // find creation paths.
    // (there could be more than one...)
    ind_beg(amc_vis::_db_node_curs, node, amc_vis::_db) {
        ind_beg(amc_vis::ctype_c_field_curs, field, *node.p_ctype) {
            amc_vis::FNode *thatnode = amc_vis::ind_node_Find(field.arg);
            if (thatnode && &node != thatnode && !thatnode->c_top && field.p_reftype->isval) {
                amc_vis::Link &link = amc_vis::link_Alloc();
                link.p_node1 = &node;
                link.p_node2 = thatnode;
                link.link    = thatnode->node;
                link.label1  = tempstr() << field.reftype << " "<<name_Get(field);
                link.label2  = thatnode->node;
                vrfy(amc_vis::link_XrefMaybe(link), algo_lib::_db.errtext);
                thatnode->c_top = &link;
            }
        }ind_end;
    }ind_end;

    // create top and bottom fields for each node (if not already there).
    ind_beg(amc_vis::_db_node_curs, node, amc_vis::_db) {
        if (!node.c_top) {
            amc_vis::Link &link = amc_vis::link_Alloc();
            link.p_node1 = &node;
            link.p_node2 = &node;
            link.link    = node.node;
            link.label2  = node.node;
            vrfy(amc_vis::link_XrefMaybe(link), algo_lib::_db.errtext);
            node.c_top = &link;
        }
        if (!node.c_bottom) {
            amc_vis::Link &link = amc_vis::link_Alloc();
            link.p_node1 = &node;
            link.p_node2 = &node;
            link.link    = tempstr() << node.node << ".<bottom>";
            vrfy(amc_vis::link_XrefMaybe(link), algo_lib::_db.errtext);
            node.c_bottom = &link;
        }
    }ind_end;

    // find access paths.
    // create a link for each field that is an x-reference.
    ind_beg(amc_vis::_db_node_curs, node, amc_vis::_db) {
        ind_beg(amc_vis::ctype_c_field_curs, field, *node.p_ctype) {
            amc_vis::FNode *thatnode = amc_vis::ind_node_Find(field.arg);
            if (DepRefQ(field) && thatnode && !field.p_reftype->isval) {
                amc_vis::Link *link = &amc_vis::link_Alloc();
                link->link           = field.field;
                link->upptr          = field.p_reftype->up;
                link->label1         = tempstr() << field.reftype << " "<<name_Get(field);
                link->p_node1        = &node;
                link->p_node2        = thatnode;
                amc_vis::link_XrefMaybe(*link);
            }
        }ind_end;
    }ind_end;

    // constrain all links to their nodes.
    ind_beg(amc_vis::_db_link_curs, link, amc_vis::_db) {
        if (&link != link.p_node1->c_top) {
            MakeABeforeB(*link.p_node1->c_top, link);
        }
        if (&link != link.p_node1->c_bottom) {
            MakeABeforeB(link, *link.p_node1->c_bottom);
        }
        if (&link != link.p_node2->c_top) {
            MakeABeforeB(*link.p_node2->c_top, link);
        }
        if (&link != link.p_node2->c_bottom) {
            MakeABeforeB(link, *link.p_node2->c_bottom);
        }
    }ind_end;

    if (amc_vis::_db.cmdline.check) {
        amc_vis::_db.cmdline.render=false;
    }

    if (amc_vis::_db.cmdline.check || amc_vis::_db.cmdline.render) {
        Main_RowLayout();
        Main_ColLayout();
    }

    if (ch_N(amc_vis::_db.cmdline.dot) > 0) {
        Main_GenerateDot();
    }

    if (amc_vis::_db.cmdline.render) {
        Main_Render();
    }
}
