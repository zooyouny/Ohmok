// OhmokAI.cpp: implementation of the COhmokAI class.
//
//////////////////////////////////////////////////////////////////////

#include "OhmokAI.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


COhmokAI::COhmokAI()
{

}

COhmokAI::~COhmokAI()
{

}

int COhmokAI::AI(int board[19][19], int stone_color, int level/*depth*/, int *x, int *y)
{
//	int x = 0, y = 0;
	//*m_board = (int*)board[0];
	for(int i = 0; i < 19 ; i++ )
		for( int j = 0 ; j < 19; j++ )
		{
			m_board[i][j] = board[i][j];
			attack_estimate[i][j] = 0;
			defence_estimate[i][j] = 0;
		}
	

	m_comp_color = stone_color;
	if(m_comp_color == BLACK_STONE)
		m_user_color = WHITE_STONE;
	if(m_comp_color == WHITE_STONE)
		m_user_color = BLACK_STONE;
	eval_pattern("10111", 1, 0x0200);
	eval_pattern("01111", 0, 0x0200);
	eval_pattern("11011", 2, 0x0200);

	eval_pattern("001110", 1, 0x0100);
	eval_pattern("010110", 2, 0x0100);

	eval_pattern("211100", 4, 0x0010);
	eval_pattern("211100", 5, 0x0010);
	eval_pattern("211010", 3, 0x0010);
	eval_pattern("211010", 5, 0x0010);
	eval_pattern("210110", 2, 0x0010);
	eval_pattern("210110", 5, 0x0010);
	eval_pattern("201110", 1, 0x0010);
	eval_pattern("11001", 2, 0x0010);
	eval_pattern("11001", 3, 0x0010);
	eval_pattern("10101", 1, 0x0010);

	eval_pattern("000110", 1, 0x0004);
	eval_pattern("001010", 1, 0x0004);
	eval_pattern("010010", 2, 0x0004);
	eval_pattern("01010", 2, 0x0004);
	eval_pattern("00110", 1, 0x0004);

	eval_pattern("211000", 3, 0x0002);
	eval_pattern("211000", 4, 0x0002);
	eval_pattern("211000", 5, 0x0002);
	eval_pattern("210100", 2, 0x0002);
	eval_pattern("210100", 4, 0x0002);
	eval_pattern("210100", 5, 0x0002);
	eval_pattern("210010", 2, 0x0002);
	eval_pattern("210010", 3, 0x0002);
	eval_pattern("210010", 5, 0x0002);

	eval_pattern("01000", 2, 0x0001);
	eval_pattern("01000", 3, 0x0001);
	eval_pattern("00100", 4, 0x0001);

	eval_samsam(m_comp_color);
	
	getBestPos(x, y);

	return 1;
}

int COhmokAI::
eval_pattern(const char *pattern, 
			 int center_pos, 
			 int score)
{
	int x, y;
	int t_flag = 0;
	for(y = 0 ; y < 19 ; y++)
	{
		for(x = 0 ; x < 19 ; x++)
		{
			t_flag = 0;
			t_flag |= check_pattern(x, y, -1, -1, pattern, m_comp_color, center_pos);
			t_flag |= (check_pattern(x, y,  0, -1, pattern, m_comp_color, center_pos)<<1);
			t_flag |= (check_pattern(x, y,  1, -1, pattern, m_comp_color, center_pos)<<2);
			t_flag |= (check_pattern(x, y,  1,  0, pattern, m_comp_color, center_pos)<<3);
			t_flag |= check_pattern(x, y,  1,  1, pattern, m_comp_color, center_pos);
			t_flag |= (check_pattern(x, y,  0,  1, pattern, m_comp_color, center_pos)<<1);
			t_flag |= (check_pattern(x, y, -1,  1, pattern, m_comp_color, center_pos)<<2);
			t_flag |= (check_pattern(x, y, -1,  0, pattern, m_comp_color, center_pos)<<3);
			if( t_flag == 1 || t_flag == 2 || t_flag == 4 || t_flag == 8 )
			{
				if(score == 0x0010 && attack_estimate[x][y] & 0x0010)
					attack_estimate[x][y] |= 0x0080;
				else if(score == 0x0010 && attack_estimate[x][y] & 0x0004)
					attack_estimate[x][y] |= 0x0040;
				else if(score == 0x0010 && attack_estimate[x][y] & 0x0002)
					attack_estimate[x][y] |= 0x0020;
				else if(score == 0x0004 && attack_estimate[x][y] & 0x0004)
					attack_estimate[x][y] |= 0x0040;
				else if(score == 0x0004 && attack_estimate[x][y] & 0x0002)
					attack_estimate[x][y] |= 0x0008;
				else
					attack_estimate[x][y] |= score;
			}
			else if( t_flag )
			{
				if(score == 0x0010)			
					attack_estimate[x][y] |= 0x0080;
				else if( score == 0x0004)
					attack_estimate[x][y] |= 0x0040;
			}
		}	
	}
	for(y = 0 ; y < 19 ; y++)
	{
		for(x = 0 ; x < 19 ; x++)
		{
			t_flag = 0;
			t_flag |= check_pattern(x, y, -1, -1, pattern, m_user_color, center_pos);
			t_flag |= (check_pattern(x, y,  0, -1, pattern, m_user_color, center_pos)<<1);
			t_flag |= (check_pattern(x, y,  1, -1, pattern, m_user_color, center_pos)<<2);
			t_flag |= (check_pattern(x, y,  1,  0, pattern, m_user_color, center_pos)<<3);
			t_flag |= check_pattern(x, y,  1,  1, pattern, m_user_color, center_pos);
			t_flag |= (check_pattern(x, y,  0,  1, pattern, m_user_color, center_pos)<<1);
			t_flag |= (check_pattern(x, y, -1,  1, pattern, m_user_color, center_pos)<<2);
			t_flag |= (check_pattern(x, y, -1,  0, pattern, m_user_color, center_pos)<<3);
			if( t_flag == 1 || t_flag == 2 || t_flag == 4 || t_flag == 8 )
			{
				if(score == 0x0010 && defence_estimate[x][y] & 0x0010)
					defence_estimate[x][y] |= 0x0080;
				else if(score == 0x0010 && defence_estimate[x][y] & 0x0004)
					defence_estimate[x][y] |= 0x0040;
				else if(score == 0x0010 && defence_estimate[x][y] & 0x0002)
					defence_estimate[x][y] |= 0x0020;
				else if(score == 0x0004 && defence_estimate[x][y] & 0x0004)
					defence_estimate[x][y] |= 0x0040;
				else if(score == 0x0004 && defence_estimate[x][y] & 0x0002)
					defence_estimate[x][y] |= 0x0008;
				else
					defence_estimate[x][y] |= score;
			}
			else if( t_flag )
			{
				if(score == 0x0010)			
					defence_estimate[x][y] |= 0x0080;
				else if( score == 0x0004)
					defence_estimate[x][y] |= 0x0040;
			}
		}	
	}
	return 1;
}

void COhmokAI::eval_samsam(int stone_color)
{
	int x, y;
	for(y = 0; y < 19; y++)
	{
		for(x = 0; x < 19; x++)
		{
			if(samsam(x, y, stone_color) == SAMSAM)
				attack_estimate[x][y] = 0;
		}
	}
}

int COhmokAI::samsam(int x, int y,int stone_color)
{
	int is_samsam = 0;
	if(check_pattern(x, y, -1,-1, "00110", stone_color, 1) 
		&& !check_pattern(x, y, -1,-1, "001101", stone_color, 1) 
		&& !check_pattern(x, y, -1,-1, "10011", stone_color, 2) )
		is_samsam++;
	if(check_pattern(x, y, 0,-1, "00110", stone_color, 1)
		&& !check_pattern(x, y,  0,-1, "001101", stone_color, 1) 
		&& !check_pattern(x, y,  0,-1, "10011", stone_color, 2) )
		is_samsam++;
	if(check_pattern(x, y, +1,-1, "00110", stone_color, 1)
		&& !check_pattern(x, y, +1,-1, "001101", stone_color, 1) 
		&& !check_pattern(x, y, +1,-1, "10011", stone_color, 2) )
		is_samsam++;
	if(check_pattern(x, y, +1,0, "00110", stone_color, 1)
		&& !check_pattern(x, y, +1, 0, "001101", stone_color, 1) 
		&& !check_pattern(x, y, +1, 0, "10011", stone_color, 2) )
		is_samsam++;
	if(check_pattern(x, y, +1,+1, "00110", stone_color, 1)
		&& !check_pattern(x, y, +1,+1, "001101", stone_color, 1) 
		&& !check_pattern(x, y, +1,+1, "10011", stone_color, 2) )
		is_samsam++;
	if(check_pattern(x, y, 0,+1, "00110", stone_color, 1)
		&& !check_pattern(x, y,  0,+1, "001101", stone_color, 1) 
		&& !check_pattern(x, y,  0,+1, "10011", stone_color, 2) )
		is_samsam++;
	if(check_pattern(x, y, -1,+1, "00110", stone_color, 1)
		&& !check_pattern(x, y, -1,+1, "001101", stone_color, 1) 
		&& !check_pattern(x, y, -1,+1, "10011", stone_color, 2) )
		is_samsam++;
	if(check_pattern(x, y, -1,0, "00110", stone_color, 1)
		&& !check_pattern(x, y, -1, 0, "001101", stone_color, 1) 
		&& !check_pattern(x, y, -1, 0, "10011", stone_color, 2) )
		is_samsam++;
    ////////////////////////////////////////////////////////////////
	if(check_pattern(x, y, -1,-1, "01010", stone_color, 2))
		if(!check_pattern(x, y, -1,-1, "010101", stone_color, 2) && 
			!check_pattern(x, y, -1,-1, "101010", stone_color, 3)) 
			is_samsam++;
	if(check_pattern(x, y, 0,-1, "01010", stone_color, 2))
		if(!check_pattern(x, y, 0,-1, "010101", stone_color, 2) && 
			!check_pattern(x, y, 0,-1, "101010", stone_color, 3)) 
		is_samsam++;
	if(check_pattern(x, y, +1,-1, "01010", stone_color, 2))
		if(!check_pattern(x, y, +1,-1, "010101", stone_color, 2) && 
			!check_pattern(x, y, +1,-1, "101010", stone_color, 3)) 
		is_samsam++;
	if(check_pattern(x, y, +1,0, "01010", stone_color, 2))
		if(!check_pattern(x, y, +1, 0, "010101", stone_color, 2) && 
			!check_pattern(x, y, +1, 0, "101010", stone_color, 3)) 
		is_samsam++;
	if(is_samsam >= 2) return SAMSAM;
	return 0;
}

BOOL COhmokAI::check_pattern(int x, int y, 
				  int increase_x, int increase_y , 
				  const char *pattern, int stone_color,int center_pos)
//center_pos은 어느 위치를 중심으로 패턴을 검색할지를 나타낸다.
{
	int i;
	int length = int(strlen(pattern));
	int stone = 0;
	int correctpattern[10]={0,};//패턴을 실제돌에 맞게 조정해줌
	x += increase_x * (-center_pos);//검색 시작 위치 조정
	y += increase_y * (-center_pos);
	
	for(i = 0 ; i < length ;i++)
	{
		if(pattern[i] == '1')
			correctpattern[i] = stone_color;
		if(pattern[i] == '2' && stone_color == BLACK_STONE)
			correctpattern[i] = WHITE_STONE;
		if(pattern[i] == '2' && stone_color == WHITE_STONE)
			correctpattern[i] = BLACK_STONE;
	}

	for(i = 0 ;i < length ;i++){
		if( x + (i * increase_x) >= 0 && x + (i * increase_x) <= 18 && 
			y + (i * increase_y) >= 0 && y + (i * increase_y) <= 18 ) {
			if(m_board[x + (i * increase_x)][y + (i * increase_y)] == correctpattern[i] ) {
				stone++;
			} 
			else break;
		}
	}
	if(length == stone) 
		return TRUE;
	return 0;
}

int COhmokAI::getBestPos(int *x, int *y)
{
	getPosToAttack(&attack);
	getPosToDefence(&defence);
	// 공격과 방어를 동시에 할 수 있는 곳을 찾는다.
	getFinalPos(attack, defence, x, y);
	
	return 1;
}

int COhmokAI::getPosToAttack(BestPos* attack)
{
	int i, j;
	for(i = 0; i < 361 ; i++)
		attack->x[i] = 0;
	for(i = 0; i < 361 ; i++)
		attack->y[i] = 0;
	attack->size = 0;
	attack->score = 0;
	for(j = 0 ; j < 19; j++)
	{
		for(i = 0 ; i < 19 ; i++)
		{
			// 아래꺼는 평가쪽에서 해야 하지 않을까?
			if(attack_estimate[i][j] != 0 && m_board[i][j] != 0)
				return 0;
			if(attack->score < attack_estimate[i][j])
			{
				attack->score = attack_estimate[i][j];
				attack->x[0] = i;
				attack->y[0] = j;
				attack->size = 1;
			}
			else if(attack->score == attack_estimate[i][j])
			{
				attack->x[attack->size] = i;
				attack->y[attack->size] = j;
				attack->size++;
			}			
		}
	}
	return 1;
}

int COhmokAI::getPosToDefence(BestPos *defence)
{
	int i, j;
	for(i = 0; i < 361 ; i++)
		defence->x[i] = 0;
	for(i = 0; i < 361 ; i++)
		defence->y[i] = 0;
	defence->size = 0;
	defence->score = 0;
	for(j = 0 ; j < 19; j++)
	{
		for(i = 0 ; i < 19 ; i++)
		{
			// 아래꺼는 평가쪽에서 해야 하지 않을까?
			if(defence_estimate[i][j] != 0 && m_board[i][j] != 0)
				return 0;
			if(defence->score < defence_estimate[i][j])
			{
				defence->score = defence_estimate[i][j];
				defence->x[0] = i;
				defence->y[0] = j;
				defence->size = 1;
			}
			else if(defence->score == defence_estimate[i][j])
			{
				defence->x[defence->size] = i;
				defence->y[defence->size] = j;
				defence->size++;
			}			
		}
	}
	return 1;

}

int COhmokAI::getFinalPos(BestPos attack, BestPos defence, int *x, int *y)
{
	int best_x[361];
	int best_y[361];
	int best_score=0;
	int best_size=0;
	int i, temp_id;
	int tmp = 0x40000000;
	int attack_is_bigger = 0;
	while( tmp != 0)
	{
		if( (attack.score & tmp) == 0 && (defence.score & tmp) == 0 )
		{
			if(tmp == 4) // 방어 4 이하는 무시한다. 공격 위주 정책을 위해 
			{
				attack_is_bigger = 1;
				break;
			}
			tmp = tmp >> 1;
		}
		else if((attack.score & tmp) != 0)
		{
			attack_is_bigger = 1;
			break;
		}
		else 
			break;
	}
	
	// 공격 하는 것중에 방어도 동시에 할 수 있는 곳에 놓는다.
	if(attack_is_bigger)
	{
		if(attack.score == 0)
		{
			if(m_board[9][9] == EMPTY_SPACE)
			{
				*x = 9;
				*y = 9;
				return 1;
			}
			else if(m_board[10][10] == EMPTY_SPACE)
			{
				*x = 10;
				*y = 10;
				return 1;
			}
		}

		for(i = 0 ; i < attack.size; i++)
		{
			if(defence_estimate[attack.x[i]][attack.y[i]] > best_score)
			{
				best_score = defence_estimate[attack.x[i]][attack.y[i]];
				best_x[0] = attack.x[i];
				best_y[0] = attack.y[i];
				best_size = 1;
			}
			else if(defence_estimate[attack.x[i]][attack.y[i]] == best_score)
			{
				best_x[best_size] = attack.x[i];
				best_y[best_size] = attack.y[i];
				best_size++;
			}
		}
	}
	// 방어 하는 것중에 공격도 동시에 할 수 있는 곳에 놓는다.
	else 
	{
		if(defence.score == 0)
		{
			if(m_board[9][9] == EMPTY_SPACE)
			{
				*x = 9;
				*y = 9;
				return 1;
			}
			else if(m_board[10][10] == EMPTY_SPACE)
			{
				*x = 10;
				*y = 10;
				return 1;
			}
		}

		for(i = 0 ; i < defence.size; i++)
		{
			if(attack_estimate[defence.x[i]][defence.y[i]] > best_score)
			{
				best_score = attack_estimate[defence.x[i]][defence.y[i]];
				best_x[0] = defence.x[i];
				best_y[0] = defence.y[i];
				best_size = 1;
			}
			else if(attack_estimate[defence.x[i]][defence.y[i]] == best_score)
			{
				best_x[best_size] = defence.x[i];
				best_y[best_size] = defence.y[i];
				best_size++;
			}
		}
	}

	srand(time(NULL));
	temp_id = rand() % best_size;
	*x = best_x[temp_id];
	*y = best_y[temp_id];
	return 1;
}
