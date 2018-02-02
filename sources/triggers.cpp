#pragma once

#ifdef _WIN32
# include "stdafx.h"
#elif __APPLE__
# include "../includes/stdafx.h"
#endif

namespace trigger
{
	namespace	tick_event
	{	
		int			next(t_env *env)
		{
			return	(on_tick(env));
		}
		int			circulate(t_tick_data *data)
		{
			data->epoch->resize(TICK_BUFF_SIZE);	//data->epoch->erase(TICK_BUFF_SIZE);
			data->px->resize(TICK_BUFF_SIZE);		//data->px->erase(TICK_BUFF_SIZE);
			data->vol->resize(TICK_BUFF_SIZE);;		//data->vol->erase(TICK_BUFF_SIZE);
			return (1);
		}
	}
	namespace	trade
	{
		int			open(
			t_env		*env,
			int16_t		order_type,		//-1 = short, +1 = long //need 2 add limit and stop orders
			float		lot_size,		//usually > 0.01 < 500
			int16_t		tp_mode,		//0 = no tp, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
			int16_t		sl_mode,		//0 = no sl, 1 = hard price, 2 = % ticker move, 3 = % trade profit to account
			float		tp,				//either 0 or hard tp (1), ticker % runup from entry (2), trade % runup from entry (3)
			float		sl,				//either 0 hard sl (1), ticker % dd from entry (2), trade % dd from entry (3)
			//int16_t	magic,			//magic number (order unique identifier)
			size_t		ticket,			//ticket number, leave 0 for default
			std::string	comment)
		{
			t_open_order order; //new order
			order.epoch_open = EPOCH(0);
			order.lot_size = lot_size;
			order.type = order_type;
			order.comment = comment;
			order.ticket = (ticket ? ticket : ((env->order_book->empty() ? 1 : env->order_book->at(0).ticket))); //default ticket is last in ticket + 1
			if (tp_mode)
			{
				if (tp_mode == 1)
					order.tp_price = tp;
				else if (tp_mode == 2)
					order.tp_price = TICK(0) * (1 + (order_type * (tp / 100)));
				else if (tp_mode == 3)
					order.tp_price = TICK(0) * 1 +
					(order_type * (env->acc->balance * (tp / 100)) / ((env->ticker->lot_size * lot_size)));
			}
			if (sl_mode)
			{
				if (sl_mode == 1)
					order.tp_price = sl;
				else if (sl_mode == 2)
					order.sl_price = TICK(0) * (1 + (-order_type * (sl / 100)));
				else if (sl_mode == 3)
					order.sl_price = TICK(0) * 1 +
					(-order_type * (env->acc->balance * (sl / 100)) / ((env->ticker->lot_size * lot_size)));
			}
			env->order_book->push_front(order);
			if (order_type == 1) NB_LONG++;
			else if (order_type == -1) NB_SHORT++;
			return (1);
		}
		int			close(
			t_env		*env,
			int16_t		sort,			//1 = FIFO, 2 = LIFO, 3 = random
			int16_t		mode,			//1 = single, 2 = all, 3 = all longs, 4 = all shorts
			size_t		ticket,			//ticket if single close, default is either oldest (FIFO) or newest (LIFO)
			std::string	comment)		//unique comment to log
		{
			//multiple open/close not yet implemented
			t_open_order closed;
			t_closed_order archive;
			for (int i = 0; i < (NB_LONG + NB_SHORT); i++)		//scan order book for matching order ticket
				if (env->order_book->at(i).ticket == ticket)
					closed = env->order_book->at(i);
			archive.type = closed.type;
			archive.epoch_open = closed.epoch_open;
			archive.epoch_create = closed.epoch_create;
			archive.epoch_close = EPOCH(0);
			archive.lot_size = closed.lot_size;
			archive.type = closed.type;
			archive.ticket = ticket;
			archive.comment = comment;
			archive.profit = closed.profit;
			archive.order_max_runup = closed.order_max_runup;
			archive.order_max_dd = closed.order_max_dd;
			env->order_history->push_front(archive);
			if (archive.type == 1) NB_LONG--;
			else if (archive.type == -1) NB_SHORT--;
			return (1);
		}
		int			modify(
			t_env		*env,			//trade environment
			size_t		ticket,			//order ticket to modify
			float		new_lot_size,	//modified order lot_size
			float		new_tp,			//modified order tp_price
			float		new_sl,			//modified order sl_price
			size_t		new_ticket,		//modified order ticket
			std::string	new_comment)	//modified order comment
		{
			//multiple open/close not yet implemented
			t_open_order order;
			for (int i = 0; i < (NB_LONG + NB_SHORT); i++)		//scan order book for matching order ticket
				if (env->order_book->at(i).ticket == ticket)
					order = env->order_book->at(i);
			if (new_lot_size)
				order.lot_size = new_lot_size;
			if (new_tp)
				order.tp_price = new_tp;
			if (new_sl)
				order.sl_price = new_sl;
			if (new_ticket)
				order.ticket = new_ticket;
			if (new_comment != "\0")
				order.comment = new_comment;
			return (1);
		}
	}
}
