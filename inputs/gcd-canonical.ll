; ModuleID = '../inputs/gcd.ll'
source_filename = "../inputs/gcd.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @gcd(i32 noundef %0, i32 noundef %1) #0 {
  %3 = icmp ne i32 %1, 0
  br i1 %3, label %.lr.ph, label %7

.lr.ph:                                           ; preds = %2
  br label %4

4:                                                ; preds = %.lr.ph, %4
  %.03 = phi i32 [ %0, %.lr.ph ], [ %.012, %4 ]
  %.012 = phi i32 [ %1, %.lr.ph ], [ %5, %4 ]
  %5 = srem i32 %.03, %.012
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %4, label %._crit_edge, !llvm.loop !6

._crit_edge:                                      ; preds = %4
  %split = phi i32 [ %.012, %4 ]
  br label %7

7:                                                ; preds = %._crit_edge, %2
  %.0.lcssa = phi i32 [ %split, %._crit_edge ], [ %0, %2 ]
  ret i32 %.0.lcssa
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
