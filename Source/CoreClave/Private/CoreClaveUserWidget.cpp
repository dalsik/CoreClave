// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreClaveUserWidget.h"

void UCoreClaveUserWidget::PlayerDrawAnimation()
{
	if (DrawAnim)
	{
		// 애니메이션 실행
		PlayAnimation(DrawAnim);
	}
}