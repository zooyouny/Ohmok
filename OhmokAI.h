// OhmokAI.h: interface for the COhmokAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OHMOKAI_H__F0D08722_97AC_441A_B1E6_C600C8E3E1A3__INCLUDED_)
#define AFX_OHMOKAI_H__F0D08722_97AC_441A_B1E6_C600C8E3E1A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include<windows.h>
#include"base.h"

typedef struct _BestPos{
	int x[361];
	int y[361];
	int score;
	int size;
}BestPos;


class COhmokAI  
{
public:
	//level수 만큼 미리 예상 해서 AI를 수행하여 평가표를 작성한다.
	int AI(int board[19][19], int stone_color, int level/*depth*/, int *x, int *y);
	
	COhmokAI();
	virtual ~COhmokAI();
	BestPos attack;
	BestPos defence;

private:
	int getBestPos(int* x, int* y);
	int getFinalPos(BestPos attack, BestPos defence, int *x, int *y);
	int getPosToDefence(BestPos* defence);
	int getPosToAttack(BestPos* attack);
	int m_board[19][19];
	int m_comp_color;
	int m_user_color;
	int attack_estimate[19][19];
	int defence_estimate[19][19];
	// 정해진 패턴을 찾아서 있으면 center_pos에 해당하는 위치에 score만큼의 점수를 OR해준다.
	int eval_pattern( const char* pattern, int center_pos, int score);
	// combi_score에 해당하는 점수 flag가 on이 되어 있으면 조합 공격으로 인식하여 score만큼의
	// 점수를 OR해준다.
	BOOL check_pattern(int x, int y, int increase_x, int increase_y , const char *pattern, int stone_color, int center_pos);
	void eval_samsam(int stone_color);
	int samsam(int x, int y,int stone_color);
};

#endif // !defined(AFX_OHMOKAI_H__F0D08722_97AC_441A_B1E6_C600C8E3E1A3__INCLUDED_)
