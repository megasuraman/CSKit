
//! @brief  BrainQuery用DataTable
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"

DEFINE_LOG_CATEGORY_STATIC(LogCSKit_BrainQueryTool, Warning, All);

#if WITH_EDITOR
/**
 * @brief 
 */
bool CSKit_BrainQueryTool::EditorCheckError(const FCSKit_BrainQueryTableRow& InData)
{
	bool bError = false;

	for (const FCSKit_BrainQueryTest& Test : InData.mTest)
	{
		if (Test.mBlackboardKeyName.IsNone())
		{
			UE_LOG(LogCSKit_BrainQueryTool, Error, TEXT("Invalid BlackboardKeyName : %s"), *Test.mTestName.ToString());
			check(0);
			bError = true;
		}
		for (const FCSKit_BrainQueryTestNode& TestNode : Test.mTestNodeList)
		{
			if (TestNode.mBrainQueryTest.Get() == nullptr)
			{
				UE_LOG(LogCSKit_BrainQueryTool, Error, TEXT("Invalid BrainQueryTest : %s / %s"), *Test.mTestName.ToString(), *TestNode.mEditorComment);
				check(0);
				bError = true;
			}
			if (TestNode.mTestPurpose == ECSKit_BrainQueryTestPurpose::Filter
				|| TestNode.mTestPurpose == ECSKit_BrainQueryTestPurpose::FilterAndScore)
			{
				if (TestNode.mTestFilterType == ECSKit_BrainQueryTestFilterType::Invalid)
				{
					UE_LOG(LogCSKit_BrainQueryTool, Error, TEXT("Invalid TestFilterType : %s / %s"), *Test.mTestName.ToString(), *TestNode.mEditorComment);
					check(0);
					bError = true;
				}
			}
		}
	}

	return bError;
}
#endif