; ModuleID = 'test_nested.ll'
source_filename = "test_nested.c"

define void @test_nested(i32 %n, i32 %m) {
entry:
  br label %outer.header

outer.header:                                 ; preds = %outer.latch, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %cmp.outer = icmp slt i32 %i.0, %n
  br i1 %cmp.outer, label %inner.header, label %outer.exit

inner.header:                                 ; preds = %inner.latch, %outer.header
  %j.0 = phi i32 [ 0, %outer.header ], [ %j.next, %inner.latch ]
  %cmp.inner = icmp slt i32 %j.0, %m
  br i1 %cmp.inner, label %inner.body, label %inner.latch

inner.body:                                   ; preds = %inner.header
  %use1 = add i32 %j.0, 1
  %use2 = add i32 %i.0, %j.0
  br label %inner.latch

inner.latch:                                  ; preds = %inner.body, %inner.header
  %j.next = add nsw i32 %j.0, 2
  br label %inner.header

outer.latch:                                  ; preds = %inner.header
  %i.next = add nsw i32 %i.0, 1
  br label %outer.header

outer.exit:                                   ; preds = %outer.header
  ret void
}
