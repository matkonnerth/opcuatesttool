import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Target } from './target';
import { Observable, of } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class TargetService {

  constructor(private http: HttpClient) { }

  private serviceUrl = '/api/targets';

  getTargets(): Observable<Target[]> {
    const targets = this.http.get<Target[]>(this.serviceUrl);
    return targets;
  }
}
