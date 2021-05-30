import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ReplComponent } from './repl.component';

describe('ReplComponent', () => {
  let component: ReplComponent;
  let fixture: ComponentFixture<ReplComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ReplComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(ReplComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
