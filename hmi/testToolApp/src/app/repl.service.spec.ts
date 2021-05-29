import { TestBed } from '@angular/core/testing';

import { ReplService } from './repl.service';

describe('ReplService', () => {
  let service: ReplService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(ReplService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
